// Copyright 2015 - 2022, GIBIS-Unifesp and the WiRedPanda contributors
// SPDX-License-Identifier: GPL-3.0-or-later

#include "srflipflop.h"

#include "qneport.h"

namespace
{
int id = qRegisterMetaType<SRFlipFlop>();
}

SRFlipFlop::SRFlipFlop(QGraphicsItem *parent)
    : GraphicElement(ElementType::SRFlipFlop, ElementGroup::Memory, 5, 5, 2, 2, parent)
{
    if (GlobalProperties::skipInit) {
        return;
    }

    m_defaultSkins << m_pixmapPath;
    m_alternativeSkins = m_defaultSkins;
    setPixmap(0);

    setRotatable(false);
    setCanChangeSkin(true);
    SRFlipFlop::updatePorts();
    setPortName(m_translatedName);
    setToolTip(m_translatedName);

    inputPort(0)->setName("S");
    inputPort(1)->setName("Clock");
    inputPort(2)->setName("R");
    inputPort(3)->setName("~Preset");
    inputPort(4)->setName("~Clear");

    outputPort(0)->setName("Q");
    outputPort(1)->setName("~Q");

    inputPort(0)->setRequired(false); /* S */
    inputPort(2)->setRequired(false); /* R */
    inputPort(3)->setRequired(false); /* p */
    inputPort(4)->setRequired(false); /* c */

    inputPort(3)->setDefaultValue(Status::Active);
    inputPort(4)->setDefaultValue(Status::Active);

    outputPort(0)->setDefaultValue(Status::Inactive);
    outputPort(1)->setDefaultValue(Status::Active);
}

void SRFlipFlop::updatePorts()
{
    inputPort(0)->setPos(0, 13);   /* S      */
    inputPort(1)->setPos(0, 29);   /* Clk    */
    inputPort(2)->setPos(0, 45);   /* R      */
    inputPort(3)->setPos(32, 0);   /* Preset */
    inputPort(4)->setPos(32, 64);  /* Clear  */

    outputPort(0)->setPos(64, 15); /* Q      */
    outputPort(1)->setPos(64, 45); /* ~Q     */
}

void SRFlipFlop::updateTheme()
{
    setPixmap(pixmapPath());
    GraphicElement::updateTheme();
}
