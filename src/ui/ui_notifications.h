#pragma once
#include <Arduino.h>
#include <lvgl.h>

enum NotificationType {
  NOTIFY_INFO,
  NOTIFY_SUCCESS,
  NOTIFY_WARNING,
  NOTIFY_ERROR,
  NOTIFY_ATTACK // Special red/hacker style
};

/**
 * @brief Push a new notification toast
 */
void ui_notification_push(const char *title, const char *msg,
                          NotificationType type);

/**
 * @brief Show Notification Center (History)
 */
void ui_notification_center_show();

/**
 * @brief Init Notification System
 */
void ui_notification_init();
