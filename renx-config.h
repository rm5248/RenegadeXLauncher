#ifndef RENXCONFIG_H
#define RENXCONFIG_H

#include <QSettings>
#include <memory>

/**
 * Get the global settings instance.
 *
 * @return
 */
std::shared_ptr<QSettings> renx_settings();

/**
 * Return the base install path of where ren-x is installed to.
 * @return
 */
QString renx_baseInstallPath();

#endif // RENXCONFIG_H
