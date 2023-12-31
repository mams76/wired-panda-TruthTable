#include "settings.h"

QString Settings::fileName()
{
    return settings->fileName();
}

QVariant Settings::value(const QString &key)
{
    return settings->value(key);
}

bool Settings::contains(const QString &key)
{
    return settings->contains(key);
}

void Settings::remove(const QString &key)
{
    settings->remove(key);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
    settings->setValue(key, value);
    settings->sync();
}
