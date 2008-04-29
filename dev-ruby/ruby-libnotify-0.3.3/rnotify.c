/***************************************************************************
 *            rnotify.c
 *
 *  Copyright (LGPL) 2006 Luca Russo
 *  vargolo@gmail.com
 ***************************************************************************/

#include "rnotify.h"

static void _rAction_callback( NotifyNotification *n,
                               const char *action, gpointer user_data );

static rAction *rAct = NULL;

/*
 * call-seq:
 *      init( name )
 *
 * name = application name
 *
 * Initialize libnotify. This must be called before any other functions
 *
 * Returns TRUE if the library initialized properly, or FALSE
 */
static VALUE
init( VALUE self, VALUE str ) {

  gboolean _init;

  if( NIL_P( str ) )
    _init = notify_init( "GenericNotifyProgram" );
  else
    _init = notify_init( StringValuePtr( str ) );

  if( _init )
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      uninit
 *
 * Deinitialize libnotify, you must to call this before quit the program
 */
static VALUE
uninit( VALUE self ) {

  notify_uninit();

  return Qnil;
}

/*
 * call-seq:
 *      init?
 *
 * Returns TRUE if libnotify is inizialized, or FALSE
 */
static VALUE
is_initted( VALUE self ) {

  if( notify_is_initted() )
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      app_name
 *
 * Returns the application name passed to Notify.init
 */
static VALUE
get_app_name( VALUE self ) {

  const gchar *name = notify_get_app_name();

  return ( rb_str_new2( name ) );
}

/*
 * call-seq:
 *      server_caps
 *
 * Queries the server for its capabilities and returns them in an Array
 */
static VALUE
get_server_caps( VALUE self ) {

  GList *caps = NULL;
  VALUE rb_caps;

  caps = notify_get_server_caps();
  rb_caps = rb_ary_new();

  do {
    rb_ary_push( rb_caps, rb_str_new2( caps->data ) );
    caps = caps->next;
  } while( caps != NULL );

  g_list_foreach( caps, (GFunc)g_free, NULL );
  g_list_free( caps );

  return rb_caps;
}

/*
 * call-seq:
 *      server_info
 *
 * Queries the server for its information( name, vendor, server version,
 * notification version )
 *
 * Returns FALSE if there were errors, an Array otherwise
 *
 * example:
 *
 *      array = Notify.server_info
 *      p array[0]      #print the product name of the server
 *      p array[1]      #print the vendor
 *      p array[2]      #print the server version
 *      p array[3]      #print the specification version supported
 */
static VALUE
get_server_info( VALUE self ) {

  gchar *serv_name = NULL,
    *vendor = NULL,
    *serv_version = NULL,
    *spec_vers = NULL;
  VALUE rb_info;

  if( !notify_get_server_info( &serv_name, &vendor, &serv_version, &spec_vers ) )
    return Qfalse;
  else {
    rb_info = rb_ary_new();
    rb_ary_push( rb_info, rb_str_new2( serv_name ) );   g_free( serv_name );
    rb_ary_push( rb_info, rb_str_new2( vendor ) );      g_free( vendor );
    rb_ary_push( rb_info, rb_str_new2( serv_version ) ); g_free( serv_version );
    rb_ary_push( rb_info, rb_str_new2( spec_vers ) );   g_free( spec_vers );
   }

  return rb_info;
}

/*
 * call-seq:
 *      new( summ, msg, icon, widget )
 *
 * summ = The summary text ( required )
 *
 * msg = The body text or nil
 *
 * icon = The icon or nil
 *
 * widget = The widget (or a Gtk::StatusIcon, when compiled against GTK+ >= 2.9.2 and libnotify >= 0.4.1) to attach to or nil
 *
 * Creates and returns a new notification
 */
static VALUE
notification_init( VALUE self, VALUE summ, VALUE msg,
                         VALUE icon, VALUE widget ) {

  char *nsumm = NULL;
  Notification *n = NULL;
  GObject *obj = NULL;

  GET_NOTIFICATION( self, n );

  nsumm = StringValuePtr( summ );

  if( nsumm == NULL || *nsumm == '\0' )
    rb_raise( rb_eArgError, "REQUIRED: the `summ` field" );

  obj = (GObject *)RVAL2GOBJ( widget );

#ifdef HAVE_STATUS_ICON
  if GTK_IS_STATUS_ICON( obj )
    n->notification = notify_notification_new_with_status_icon( nsumm,
                                                                NIL_P( msg ) ? NULL : StringValuePtr( msg ),
                                                                NIL_P( icon ) ? NULL : StringValuePtr( icon ),
                                                                (GtkStatusIcon *)obj );
  else
    n->notification = notify_notification_new( nsumm,
                                               NIL_P( msg ) ? NULL : StringValuePtr( msg ),
                                               NIL_P( icon ) ? NULL : StringValuePtr( icon ),
                                               (GtkWidget *)obj );
#else
  n->notification = notify_notification_new( nsumm,
                                             NIL_P( msg ) ? NULL : StringValuePtr( msg ),
                                             NIL_P( icon ) ? NULL : StringValuePtr( icon ),
                                             (GtkWidget *)obj );
#endif
  if( n->notification == NULL )
    rb_raise( rb_eRuntimeError, "Can not create a new notification" );

  return self;
}

/*
 * call-seq:
 *      update( summ, msg, icon )
 *
 * summ = The new summary text ( required )
 *
 * msg = The new body text or nil
 *
 * icon = The new icon or nil
 *
 * This won't send the update out and display it on the screen.
 * For that, you will need to call the show method.
 *
 * return TRUE if ok or FALSE otherwise
 */
static VALUE
notification_update( VALUE self, VALUE summ, VALUE msg, VALUE icon ) {

  char *nsumm = NULL;
  gboolean ok;
  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  nsumm = StringValuePtr( summ );

  if( nsumm == NULL || *nsumm == '\0' )
    rb_raise( rb_eArgError, "REQUIRED: the `summ` field" );

  ok = notify_notification_update( n->notification, nsumm,
                                   NIL_P( msg ) ? NULL : StringValuePtr( msg ),
                                   NIL_P( icon ) ? NULL : StringValuePtr( icon ) );

  if( ok )
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      attach_to( widget )
 *
 * widget = The widget (or a Gtk::StatusIcon, when compiled against GTK+ >= 2.9.2 and libnotify >= 0.4.1) to attach to
 *
 * Attaches the notification to a widget/Gtk::StatusIcon
 */
static VALUE
notification_attach( VALUE self, VALUE widget ) {

  Notification *n = NULL;
  GObject *obj = NULL;

  GET_NOTIFICATION( self, n );

  obj = (GObject *)RVAL2GOBJ( widget );

#ifdef HAVE_STATUS_ICON
  if GTK_IS_STATUS_ICON( obj )
                         notify_notification_attach_to_status_icon( n->notification,
                                                                    (GtkStatusIcon *)obj );
  else
    notify_notification_attach_to_widget( n->notification,
                                          (GtkWidget *)obj );
#else
  notify_notification_attach_to_widget( n->notification,
                                        (GtkWidget *)obj );
#endif

  return Qnil;
}

/*
 * call-seq:
 *      show
 *
 * Tells the notification server to display the notification on the screen.
 * if TRUE returns, show the notification otherwise returns FALSE
 */
static VALUE
notification_show( VALUE self ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  if( notify_notification_show( n->notification, NULL ) )
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      timeout=( milliseconds )
 *
 * Sets the timeout in milliseconds.
 */
static VALUE
notification_set_timeout( VALUE self, VALUE ml ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_timeout( n->notification, FIX2INT( ml ) );

  return Qnil;
}

/*
 * call-seq:
 *      category=( cat_name )
 *
 * cat_name = category name
 *
 * Sets the category
 */
static VALUE
notification_set_category( VALUE self, VALUE cat ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_category( n->notification, StringValuePtr( cat ) );

  return Qnil;
}

/*
 * call-seq:
 *      urgency=( urg_level )
 *
 * urg_level = urgency level
 *
 * Sets the urgency level
 */
static VALUE
notification_set_urgency( VALUE self, VALUE urg ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  switch( FIX2INT( urg ) ) {
  case 0: notify_notification_set_urgency( n->notification,
                                           NOTIFY_URGENCY_LOW );
    break;
  case 1: notify_notification_set_urgency( n->notification,
                                           NOTIFY_URGENCY_NORMAL );
    break;
  case 2: notify_notification_set_urgency( n->notification,
                                           NOTIFY_URGENCY_CRITICAL );
  }

  return Qnil;
}

/*
 * call-seq:
 *      pixbuf_icon=( icon )
 *
 * icon = The icon
 *
 * Sets the icon from a GdkPixbuf
 */
static VALUE
notification_set_icon( VALUE self, VALUE icon ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_icon_from_pixbuf( n->notification,
                                            (GdkPixbuf *)RVAL2GOBJ( icon ) );

  return Qnil;
}

/*
 * call-seq:
 *      hint32( key, val )
 *
 * key = The hint
 *
 * val = The hint's value
 *
 * Sets a hint with a 32-bit integer value
 */
static VALUE
notification_set_hint32( VALUE self, VALUE key, VALUE val ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_hint_int32( n->notification,
                                      StringValuePtr( key ), FIX2INT( val ) );

  return Qnil;
}

/*
 * call-seq:
 *      hint_double( key, val )
 *
 * key = The hint
 *
 * val = The hint's value
 *
 * Sets a hint with a double value
 */
static VALUE
notification_set_hint_double( VALUE self, VALUE key, VALUE val ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_hint_double( n->notification,
                                       StringValuePtr( key ),
                                       NUM2DBL( val ) );

  return Qnil;
}

/*
 * call-seq:
 *      hint_string( key, val )
 *
 * key = The hint
 *
 * val = The hint's value
 *
 * Sets a hint with a string value
 */
static VALUE
notification_set_hint_string( VALUE self, VALUE key, VALUE val ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_hint_string( n->notification, StringValuePtr( key ),
                                       StringValuePtr( val ) );

  return Qnil;
}

/*
 * call-seq:
 *      hint_byte( key, val )
 *
 * key = The hint
 *
 * val = The hint's value
 *
 * Sets a hint with a byte value
 */
static VALUE
notification_set_hint_byte( VALUE self, VALUE key, VALUE val ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_set_hint_byte( n->notification,
                                     StringValuePtr( key ), FIX2INT( val ) );

  return Qnil;
}

/*
 * call-seq:
 *      clear_hints
 *
 * Clears all hints from the notification
 */
static VALUE
notification_clear_hints( VALUE self ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_clear_hints( n->notification );

  return Qnil;
}

/*
 * call-seq:
 *      close
 *
 * Tells the notification server to hide the notification on the screen
 */
static VALUE
notification_close( VALUE self ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  if( notify_notification_close( n->notification, NULL ) )
    return Qtrue;

  return Qfalse;
}

/*
 * call-seq:
 *      clear_actions
 *
 * Clears all actions from the notification
 */
static VALUE
notification_clear_actions( VALUE self ) {

  Notification *n = NULL;

  GET_NOTIFICATION( self, n );

  notify_notification_clear_actions( n->notification );

  _rAction_free_all( n->actions );

  return Qnil;
}

#ifdef HAVE_GEOMETRY_HINTS
/*
 * call-seq:
 *      geometry_hints( screen, x, y )
 *
 * ** WHEN COMPILED AGAINST LIBNOTIFY 0.4.1 OR HIGHER **
 *
 * screen = The GdkScreen the notification should appear on
 *
 * x = The X coordinate to point to
 *
 * y = The Y coordinate to point to
 *
 * Sets the geometry hints on the notification
 */
static VALUE
notification_set_geometry_hints( VALUE self, VALUE screen, VALUE x,
                                  VALUE y ) {

  GdkScreen *sc = NULL;
  Notification *n = NULL;

  sc = (GdkScreen *)RVAL2GOBJ( screen );
  if( !sc )
    rb_raise( rb_eArgError, "REQUIRED: the `screen` field" );

  GET_NOTIFICATION( self, n );

  notify_notification_set_geometry_hints( n->notification,
                                          sc, FIX2INT( x ), FIX2INT( y ) );

  return Qnil;
}
#endif

/*
 * call-seq:
 *      add_action( action, label, user_data ) { |action, user_data| ... }
 *
 * action = The action id
 *
 * label = The action label
 *
 * user_data = Custom data to pass into the block ( optional )
 *
 * Adds an action. When the action is invoked, the specified block will be called
 *
 * Examples:
 *
 * myinstance.add_action( "MyAction", "MyLabel" ) do |action|
 *      # something to do
 * end
 *
 * Or
 *
 * myinstance.add_action( "MyAction", "MyLabel", MyData ) do |action, mydata|
 *      # something to do
 * end
 */
static VALUE
notification_add_action( int argc, VALUE *argv, VALUE self ) {

  Notification *n = NULL;

  static int pid = 0;   /* private_id */

  VALUE action;
  VALUE label;
  VALUE data;
  VALUE body;

  if( !rb_block_given_p() ) {
    rb_raise( rb_eRuntimeError, "This method requires a block" );
  }

  GET_NOTIFICATION( self, n );

  rb_scan_args( argc, argv, "21&", &action, &label, &data, &body );

  n->actions = _rAction_add( n->actions, action, data, body, pid );

  rAct = n->actions;

  notify_notification_add_action( n->notification,
                                  StringValuePtr( action ),
                                  StringValuePtr( label ),
                                  (NotifyActionCallback) _rAction_callback,
                                  (gpointer)pid, NULL );
  pid++;

  return Qnil;
}

/*
 * libnotify ruby interface
 * [ http://www.galago-project.org ]
 */
void
Init_rnotify() {

  VALUE mNotify;
  VALUE cNotification;

  mNotify = rb_define_module( "Notify" );
  cNotification = rb_define_class_under( mNotify, "Notification", rb_cObject );
  rb_define_alloc_func( cNotification, _wrap_alloc );

  rb_define_const( cNotification, "URGENCY_LOW", INT2FIX( NOTIFY_URGENCY_LOW ) );
  rb_define_const( cNotification, "URGENCY_NORMAL", INT2FIX( NOTIFY_URGENCY_NORMAL ) );
  rb_define_const( cNotification, "URGENCY_CRITICAL", INT2FIX( NOTIFY_URGENCY_CRITICAL ) );
  rb_define_const( cNotification, "EXPIRES_DEFAULT", INT2FIX( NOTIFY_EXPIRES_DEFAULT ) );
  rb_define_const( cNotification, "EXPIRES_NEVER", INT2FIX( NOTIFY_EXPIRES_NEVER ) );

  rb_define_module_function( mNotify, "init", init, 1 );
  rb_define_module_function( mNotify, "uninit", uninit, 0 );
  rb_define_module_function( mNotify, "init?", is_initted, 0 );
  rb_define_module_function( mNotify, "app_name", get_app_name, 0 );
  rb_define_module_function( mNotify, "server_caps", get_server_caps, 0 );
  rb_define_module_function( mNotify, "server_info", get_server_info, 0 );

  rb_define_method( cNotification, "initialize", notification_init, 4 );
  rb_define_method( cNotification, "update", notification_update, 3 );
  rb_define_method( cNotification, "attach_to", notification_attach, 1 );
  rb_define_method( cNotification, "show", notification_show, 0 );
  rb_define_method( cNotification, "timeout=", notification_set_timeout, 1 );
  rb_define_method( cNotification, "category=", notification_set_category, 1 );
  rb_define_method( cNotification, "urgency=", notification_set_urgency, 1 );
  rb_define_method( cNotification, "pixbuf_icon=", notification_set_icon, 1 );
  rb_define_method( cNotification, "hint32", notification_set_hint32, 2 );
  rb_define_method( cNotification, "hint_double", notification_set_hint_double, 2 );
  rb_define_method( cNotification, "hint_string", notification_set_hint_string, 2 );
  rb_define_method( cNotification, "hint_byte", notification_set_hint_byte, 2 );
#ifdef HAVE_GEOMETRY_HINTS
  rb_define_method( cNotification, "geometry_hints", notification_set_geometry_hints, 3 );
#endif
  rb_define_method( cNotification, "add_action", notification_add_action, -1 );
  rb_define_method( cNotification, "clear_actions", notification_clear_actions, 0 );
  rb_define_method( cNotification, "clear_hints", notification_clear_hints, 0 );
  rb_define_method( cNotification, "close", notification_close, 0 );
}

static void
_rAction_callback( NotifyNotification *n, const char *action, gpointer user_data ) {

  rAction *tmp = NULL;
  int pid = (int *)user_data;

  tmp = _rAction_find( rAct, pid );

  if( tmp->udata == Qnil )
    rb_funcall( tmp->body, rb_intern( "call" ), 1, tmp->action );
  else
    rb_funcall( tmp->body, rb_intern( "call" ), 2, tmp->action, tmp->udata );
}
