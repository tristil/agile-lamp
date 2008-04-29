
#ifndef __notify_marshal_MARSHAL_H__
#define __notify_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:UINT,STRING (notify-marshal.list:1) */
extern void notify_marshal_VOID__UINT_STRING (GClosure     *closure,
                                              GValue       *return_value,
                                              guint         n_param_values,
                                              const GValue *param_values,
                                              gpointer      invocation_hint,
                                              gpointer      marshal_data);

G_END_DECLS

#endif /* __notify_marshal_MARSHAL_H__ */

