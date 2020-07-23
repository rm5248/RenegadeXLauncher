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

#endif // RENXCONFIG_H
