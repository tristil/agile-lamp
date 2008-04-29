
/* Generated data (by glib-mkenums) */

#include <libnotify/notify.h>

/* enumerations from "notification.h" */
GType
notify_urgency_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { NOTIFY_URGENCY_LOW, "NOTIFY_URGENCY_LOW", "low" },
      { NOTIFY_URGENCY_NORMAL, "NOTIFY_URGENCY_NORMAL", "normal" },
      { NOTIFY_URGENCY_CRITICAL, "NOTIFY_URGENCY_CRITICAL", "critical" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("NotifyUrgency", values);
  }
  return etype;
}

/* Generated data ends here */

