#include "simplewaveform.h"
#include "ui_simplewaveform.h"

#include <QChartView>
#include <QDebug>
#include <QLineSeries>
#include <cmath>
#include <input.h>

#include <QPointF>
#include <QSettings>
#include <QValueAxis>
#include <bitset>

using namespace QtCharts;


SimpleWaveform::SimpleWaveform( Editor *editor, QWidget *parent ) :
  editor( editor ),
  QDialog( parent ),
  ui( new Ui::SimpleWaveform ) {
  ui->setupUi( this );
  resize( 800, 500 );
/*  chart.legend( )->hide( ); */
/*  chart->addSeries(series); */

/*  chart.setTitle( "Simple Waveform View." ); */

  QChartView *chartView = new QChartView( &chart, this );
  chartView->setRenderHint( QPainter::Antialiasing );
  ui->gridLayout->addWidget( chartView );
  setWindowTitle( "Simple WaveForm - WaveDolphin Beta" );

  sortingType = SortingType::INCREASING;

}

SimpleWaveform::~SimpleWaveform( ) {
  delete ui;
}

void SimpleWaveform::showWaveform( ) {
  QSettings settings( QSettings::IniFormat, QSettings::UserScope,
                      QApplication::organizationName( ), QApplication::applicationName( ) );
  settings.beginGroup( "waveform" );
  if( settings.contains( "sortingType" ) ) {
    sortingType = ( SortingType ) settings.value( "sortingType" ).toInt( );
  }
  settings.endGroup( );
  switch( sortingType ) {
      case SortingType::DECREASING:
      ui->radioButton_Decreasing->setChecked( true );
      break;
      case SortingType::INCREASING:
      ui->radioButton_Increasing->setChecked( true );
      break;
      case SortingType::POSITION:
      ui->radioButton_Position->setChecked( true );
      break;
  }
  int gap = 3;

  chart.removeAllSeries( );

  QVector< GraphicElement* > elements = editor->getScene( )->getElements( );
  if( elements.isEmpty( ) ) {
    return;
  }
  SimulationController *sc = editor->getSimulationController( );
  sc->stop( );

  QVector< GraphicElement* > inputs;
  QVector< GraphicElement* > outputs;

  QVector< QLineSeries* > in_series;
  QVector< QLineSeries* > out_series;
  for( GraphicElement *elm : elements ) {
    switch( elm->elementType( ) ) {
        case ElementType::BUTTON:
        case ElementType::SWITCH: {
        inputs.append( elm );
        break;
      }
        case ElementType::LED: {
        outputs.append( elm );
        break;
      }
        default:
        break;
    }
  }
  if( inputs.isEmpty( ) ) {
    return;
  }
  if( sortingType == SortingType::INCREASING ) {
    std::sort( inputs.begin( ), inputs.end( ), [ ]( GraphicElement *elm1, GraphicElement *elm2 ) {
      return( strcasecmp( elm1->getLabel( ).toUtf8( ), elm2->getLabel( ).toUtf8( ) ) <= 0 );
    } );
    std::sort( outputs.begin( ), outputs.end( ), [ ]( GraphicElement *elm1, GraphicElement *elm2 ) {
      return( strcasecmp( elm1->getLabel( ).toUtf8( ), elm2->getLabel( ).toUtf8( ) ) <= 0 );
    } );
  }
  else if( sortingType == SortingType::DECREASING ) {
    std::sort( inputs.begin( ), inputs.end( ), [ ]( GraphicElement *elm1, GraphicElement *elm2 ) {
      return( strcasecmp( elm1->getLabel( ).toUtf8( ), elm2->getLabel( ).toUtf8( ) ) >= 0 );
    } );
    std::sort( outputs.begin( ), outputs.end( ), [ ]( GraphicElement *elm1, GraphicElement *elm2 ) {
      return( strcasecmp( elm1->getLabel( ).toUtf8( ), elm2->getLabel( ).toUtf8( ) ) >= 0 );
    } );
  }
  else {
    std::sort( inputs.begin( ), inputs.end( ), [ ]( GraphicElement *elm1, GraphicElement *elm2 ) {
      return( elm1->pos( ).ry( ) < elm2->pos( ).ry( ) );
    } );
    std::sort( outputs.begin( ), outputs.end( ), [ ]( GraphicElement *elm1, GraphicElement *elm2 ) {
      return( elm1->pos( ).ry( ) < elm2->pos( ).ry( ) );
    } );
  }
  QVector< char > oldValues( inputs.size( ) );
  for( int in = 0; in < inputs.size( ); ++in ) {
    in_series.append( new QLineSeries( this ) );
    in_series[ in ]->setName( inputs[ in ]->getLabel( ) );
    oldValues[ in ] = inputs[ in ]->output( )->value( );
    chart.addSeries( in_series[ in ] );
  }
  for( int out = 0; out < outputs.size( ); ++out ) {
    out_series.append( new QLineSeries( this ) );
    out_series[ out ]->setName( outputs[ out ]->getLabel( ) );
    chart.addSeries( out_series[ out ] );
  }
  int num_iter = pow( 2, inputs.size( ) );
/*  qDebug( ) << "Num iter = " << num_iter; */
  for( int itr = 0; itr < num_iter; ++itr ) {
    std::bitset< std::numeric_limits< unsigned int >::digits > bs( itr );
    for( int in = 0; in < inputs.size( ); ++in ) {
      dynamic_cast< Input* >( inputs[ in ] )->setOn( bs[ in ] );
      int offset = ( inputs.size( ) - in - 1 ) * 2 + outputs.size( ) * 2 + gap + 1;
      in_series[ in ]->append( itr, offset + bs[ inputs.size( ) - in - 1 ] );
      in_series[ in ]->append( itr + 1, offset + bs[ inputs.size( ) - in - 1 ] );
    }
    for( GraphicElement *elm : elements ) {
      elm->updateLogic( );
    }
    for( int out = 0; out < outputs.size( ); ++out ) {
      int val = outputs[ out ]->input( )->value( );
      int offset = ( outputs.size( ) - out - 1 ) * 2 + 1;
      out_series[ out ]->append( itr, offset + val );
      out_series[ out ]->append( itr + 1, offset + val );
    }
  }
  chart.createDefaultAxes( );

/*  chart.axisY( )->hide( ); */
  QValueAxis *ax = dynamic_cast< QValueAxis* >( chart.axisX( ) );
  ax->setRange( 0, num_iter );
  ax->setTickCount( num_iter + 1 );
  QValueAxis *ay = dynamic_cast< QValueAxis* >( chart.axisY( ) );
  ay->setShadesBrush( QBrush( Qt::lightGray ) );
  ay->setTickCount( inputs.size( ) * 2 + outputs.size( ) * 2 + gap + 1 );
  ay->setRange( 0, inputs.size( ) * 2 + outputs.size( ) * 2 + gap + 1 );
  ay->hide( );

  show( );
  for( int in = 0; in < inputs.size( ); ++in ) {
    dynamic_cast< Input* >( inputs[ in ] )->setOn( oldValues[ in ] );

  }
  sc->start( );
}

void SimpleWaveform::on_radioButton_Position_clicked( ) {
  QSettings settings( QSettings::IniFormat, QSettings::UserScope,
                      QApplication::organizationName( ), QApplication::applicationName( ) );
  settings.beginGroup( "waveform" );
  sortingType = SortingType::POSITION;
  settings.setValue( "sortingType", ( int ) sortingType );
  settings.endGroup( );

  showWaveform( );
}

void SimpleWaveform::on_radioButton_Increasing_clicked( ) {
  QSettings settings( QSettings::IniFormat, QSettings::UserScope,
                      QApplication::organizationName( ), QApplication::applicationName( ) );
  settings.beginGroup( "waveform" );
  sortingType = SortingType::INCREASING;
  settings.setValue( "sortingType", ( int ) sortingType );
  settings.endGroup( );

  showWaveform( );
}

void SimpleWaveform::on_radioButton_Decreasing_clicked( ) {
  QSettings settings( QSettings::IniFormat, QSettings::UserScope,
                      QApplication::organizationName( ), QApplication::applicationName( ) );
  settings.beginGroup( "waveform" );
  sortingType = SortingType::DECREASING;
  settings.setValue( "sortingType", ( int ) sortingType );
  settings.endGroup( );

  showWaveform( );
}