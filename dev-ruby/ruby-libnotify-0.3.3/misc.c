/*
 * part of ruby-libnotify
 */

#include "rnotify.h"

/* alloc */

static void
_wrap_alloc_free( void *pthis ) {

  Notification *n = (Notification *)pthis;

  g_object_unref( G_OBJECT( n->notification ) );
  n->notification = NULL;
}

VALUE
_wrap_alloc( VALUE klass ) {

  Notification *tmp = NULL;

  tmp = ALLOC_N( Notification, 1 );
  tmp->notification = NULL;
  tmp->actions = NULL;

  return Data_Wrap_Struct( klass, 0, _wrap_alloc_free, tmp );
}

/* Action List */

rAction *
_rAction_add( rAction *pthis, VALUE action, VALUE data, VALUE body, int id ) {

  rAction *tmp = NULL;
  rAction *tmp2 = NULL;

  tmp = (rAction *)malloc( sizeof( rAction ) );
  if( !tmp )
    rb_raise( rb_eRuntimeError,
              "ERROR: Cannot add a new action! ( ID: %d )\n", id );

  tmp->action = action;
  tmp->udata = NIL_P( data ) ? Qnil : data;
  tmp->body = body;
  tmp->id = id;
  tmp->next = NULL;
  if( pthis == NULL ) {
    pthis = tmp;
  } else {
    for( tmp2 = pthis ; tmp2->next != NULL; tmp2 = tmp2->next );
    tmp2->next = tmp;
  }

  return pthis;
}

rAction *
_rAction_find( rAction *pthis, int id ) {

  if( !pthis->next || pthis->id == id )
    return pthis;

  return _rAction_find( pthis->next, id );
}

void
_rAction_free_all( rAction *pthis ) {

  if( pthis ) {
    _rAction_free_all( pthis->next );
    free( pthis );
  }
}
