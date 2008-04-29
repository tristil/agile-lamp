/***************************************************************************
 *            rnotify.h
 *
 *  Copyright (LGPL) 2006 Luca Russo
 *  vargolo@gmail.com
 ***************************************************************************/

#ifndef RNOTIFY_H
#define RNOTIFY_H

#include <libnotify/notify.h>

#include "ruby.h"
#include "rbgobject.h"

typedef struct _rAction rAction;
typedef struct _Notification Notification;

struct _rAction {
  VALUE action;
  VALUE udata;
  VALUE body;
  int id;
  rAction *next;
};

struct _Notification {
  NotifyNotification *notification;
  rAction *actions;
};

#define GET_NOTIFICATION( o, r ) \
  Data_Get_Struct( o, Notification, r )

/* misc.c */
VALUE _wrap_alloc( VALUE klass );

rAction *_rAction_add( rAction *pthis, VALUE action, VALUE data,
                       VALUE body, int id );
rAction *_rAction_find( rAction *pthis, int id );
void _rAction_free_all( rAction *pthis );

#endif /* RNOTIFY_H */
