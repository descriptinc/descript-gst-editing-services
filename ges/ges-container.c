/* GStreamer Editing Services
 * Copyright (C) <2013> Thibault Saunier <thibault.saunier@collabora.com>
 *               <2013> Collabora Ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/**
 * SECTION:gescontainer
 * @title: GESContainer
 * @short_description: Base Class for objects responsible for controlling other
 * GESTimelineElement-s
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "ges-container.h"
#include "ges.h"
#include "ges-internal.h"

#include <string.h>

GST_DEBUG_CATEGORY_STATIC (ges_container_debug);
#undef GST_CAT_DEFAULT
#define GST_CAT_DEFAULT ges_container_debug

/* Mapping of relationship between a Container and the TimelineElements
 * it controls
 *
 * NOTE : Does it make sense to make it public in the future ?
 */
typedef struct
{
  GESTimelineElement *child;

  GstClockTime start_offset;
  GstClockTime duration_offset;
  GstClockTime inpoint_offset;
  gint32 priority_offset;

  guint start_notifyid;
  guint duration_notifyid;
  guint inpoint_notifyid;
} ChildMapping;

enum
{
  CHILD_ADDED_SIGNAL,
  CHILD_REMOVED_SIGNAL,
  LAST_SIGNAL
};

static guint ges_container_signals[LAST_SIGNAL] = { 0 };

struct _GESContainerPrivate
{
  /*< public > */
  GESLayer *layer;

  /*< private > */
  /* Set to TRUE when the container is doing updates of track object
   * properties so we don't end up in infinite property update loops
   */
  GHashTable *mappings;

  /* List of GESTimelineElement being in the "child-added" signal
   * emission stage */
  GList *adding_children;

  GList *copied_children;
};

enum
{
  PROP_0,
  PROP_HEIGHT,
  PROP_LAST
};

static GParamSpec *properties[PROP_LAST];

G_DEFINE_ABSTRACT_TYPE_WITH_PRIVATE (GESContainer, ges_container,
    GES_TYPE_TIMELINE_ELEMENT);

/************************
 *   Private  methods   *
 ************************/
static void
_free_mapping (ChildMapping * mapping)
{
  GESTimelineElement *child = mapping->child;

  /* Disconnect all notify listeners */
  if (mapping->start_notifyid)
    g_signal_handler_disconnect (child, mapping->start_notifyid);
  if (mapping->duration_notifyid)
    g_signal_handler_disconnect (child, mapping->duration_notifyid);
  if (mapping->inpoint_notifyid)
    g_signal_handler_disconnect (child, mapping->inpoint_notifyid);

  ges_timeline_element_set_parent (child, NULL);
  g_slice_free (ChildMapping, mapping);
}

static gint
compare_grouping_prio (GType * a, GType * b)
{
  gint ret = 0;
  GObjectClass *aclass = g_type_class_ref (*a);
  GObjectClass *bclass = g_type_class_ref (*b);

  /* We want higher prios to be first */
  if (GES_CONTAINER_CLASS (aclass)->grouping_priority <
      GES_CONTAINER_CLASS (bclass)->grouping_priority)
    ret = 1;
  else if (GES_CONTAINER_CLASS (aclass)->grouping_priority >
      GES_CONTAINER_CLASS (bclass)->grouping_priority)
    ret = -1;

  g_type_class_unref (aclass);
  g_type_class_unref (bclass);
  return ret;
}

static void
_resync_start_offsets (GESTimelineElement * child,
    ChildMapping * map, GESContainer * container)
{
  map->start_offset = _START (container) - _START (child);
}

/*****************************************************
 *                                                   *
 * GESTimelineElement virtual methods implementation *
 *                                                   *
 *****************************************************/
static gboolean
_set_start (GESTimelineElement * element, GstClockTime start)
{
  GList *tmp;
  ChildMapping *map;
  GESContainer *container = GES_CONTAINER (element);
  GESContainerPrivate *priv = container->priv;

  GST_DEBUG_OBJECT (element, "Updating children offsets, (initiated_move: %"
      GST_PTR_FORMAT ")", container->initiated_move);

  for (tmp = container->children; tmp; tmp = g_list_next (tmp)) {
    GESTimelineElement *child = (GESTimelineElement *) tmp->data;

    map = g_hash_table_lookup (priv->mappings, child);
    map->start_offset = start - _START (child);
  }
  container->children_control_mode = GES_CHILDREN_UPDATE;

  return TRUE;
}

static gboolean
_set_inpoint (GESTimelineElement * element, GstClockTime inpoint)
{
  GList *tmp;
  GESContainer *container = GES_CONTAINER (element);

  for (tmp = container->children; tmp; tmp = g_list_next (tmp)) {
    GESTimelineElement *child = (GESTimelineElement *) tmp->data;
    ChildMapping *map = g_hash_table_lookup (container->priv->mappings, child);

    map->inpoint_offset = inpoint - _INPOINT (child);
  }

  return TRUE;
}

static gboolean
_set_duration (GESTimelineElement * element, GstClockTime duration)
{
  GList *tmp;
  GESContainer *container = GES_CONTAINER (element);
  GESContainerPrivate *priv = container->priv;

  for (tmp = container->children; tmp; tmp = g_list_next (tmp)) {
    GESTimelineElement *child = (GESTimelineElement *) tmp->data;
    ChildMapping *map = g_hash_table_lookup (priv->mappings, child);

    map->duration_offset = duration - _DURATION (child);
  }

  return TRUE;
}

static void
_ges_container_add_child_properties (GESContainer * container,
    GESTimelineElement * child)
{
  guint n_props, i;

  GParamSpec **child_props =
      ges_timeline_element_list_children_properties (child,
      &n_props);

  for (i = 0; i < n_props; i++) {
    GObject *prop_child;
    gchar *prop_name = g_strdup_printf ("%s::%s",
        g_type_name (child_props[i]->owner_type),
        child_props[i]->name);

    if (ges_timeline_element_lookup_child (child, prop_name, &prop_child, NULL)) {
      ges_timeline_element_add_child_property (GES_TIMELINE_ELEMENT (container),
          child_props[i], prop_child);
      gst_object_unref (prop_child);

    }
    g_free (prop_name);
    g_param_spec_unref (child_props[i]);
  }

  g_free (child_props);
}

static void
_ges_container_remove_child_properties (GESContainer * container,
    GESTimelineElement * child)
{
  guint n_props, i;

  GParamSpec **child_props =
      ges_timeline_element_list_children_properties (child,
      &n_props);

  for (i = 0; i < n_props; i++) {
    GObject *prop_child;
    gchar *prop_name = g_strdup_printf ("%s::%s",
        g_type_name (child_props[i]->owner_type),
        child_props[i]->name);

    if (ges_timeline_element_lookup_child (child, prop_name, &prop_child, NULL)) {
      ges_timeline_element_remove_child_property (GES_TIMELINE_ELEMENT
          (container), child_props[i]);
      gst_object_unref (prop_child);

    }

    g_free (prop_name);
    g_param_spec_unref (child_props[i]);
  }

  g_free (child_props);
}

static GParamSpec **
_list_children_properties (GESTimelineElement * self, guint * n_properties)
{
  GList *tmp;

  for (tmp = GES_CONTAINER_CHILDREN (self); tmp; tmp = tmp->next)
    _ges_container_add_child_properties (GES_CONTAINER (self), tmp->data);

  return
      GES_TIMELINE_ELEMENT_CLASS
      (ges_container_parent_class)->list_children_properties (self,
      n_properties);
}

static gboolean
_lookup_child (GESTimelineElement * self, const gchar * prop_name,
    GObject ** child, GParamSpec ** pspec)
{
  GList *tmp;

  /* FIXME Implement a syntax to precisely get properties by path */
  for (tmp = GES_CONTAINER_CHILDREN (self); tmp; tmp = tmp->next) {
    if (ges_timeline_element_lookup_child (tmp->data, prop_name, child, pspec))
      return TRUE;
  }

  return FALSE;
}

static GESTrackType
_get_track_types (GESTimelineElement * object)
{
  GESTrackType types = GES_TRACK_TYPE_UNKNOWN;
  GList *tmp, *children = ges_container_get_children (GES_CONTAINER (object),
      TRUE);

  for (tmp = children; tmp; tmp = tmp->next) {
    if (GES_IS_TRACK_ELEMENT (tmp->data)) {
      types |= ges_timeline_element_get_track_types (tmp->data);
    }
  }

  g_list_free_full (children, gst_object_unref);

  return types ^ GES_TRACK_TYPE_UNKNOWN;
}

static void
_deep_copy (GESTimelineElement * element, GESTimelineElement * copy)
{
  GList *tmp;
  GESContainer *self = GES_CONTAINER (element), *ccopy = GES_CONTAINER (copy);

  for (tmp = GES_CONTAINER_CHILDREN (element); tmp; tmp = tmp->next) {
    ChildMapping *map;

    map =
        g_slice_dup (ChildMapping, g_hash_table_lookup (self->priv->mappings,
            tmp->data));
    map->child = ges_timeline_element_copy (tmp->data, TRUE);
    map->start_notifyid = 0;
    map->inpoint_notifyid = 0;
    map->duration_notifyid = 0;

    ccopy->priv->copied_children = g_list_prepend (ccopy->priv->copied_children,
        map);
  }
}

static GESTimelineElement *
_paste (GESTimelineElement * element, GESTimelineElement * ref,
    GstClockTime paste_position)
{
  GList *tmp;
  ChildMapping *map;
  GESContainer *ncontainer =
      GES_CONTAINER (ges_timeline_element_copy (element, FALSE));
  GESContainer *self = GES_CONTAINER (element);

  for (tmp = self->priv->copied_children; tmp; tmp = tmp->next) {
    GESTimelineElement *nchild;

    map = tmp->data;
    nchild =
        ges_timeline_element_paste (map->child,
        paste_position - map->start_offset);

    if (!nchild) {
      while (ncontainer->children)
        ges_container_remove (ncontainer, ncontainer->children->data);

      g_object_unref (ncontainer);
      return NULL;
    }

    ges_timeline_element_set_timeline (GES_TIMELINE_ELEMENT (ncontainer),
        GES_TIMELINE_ELEMENT_TIMELINE (ref));
    ges_container_add (ncontainer, nchild);
  }

  return GES_TIMELINE_ELEMENT (ncontainer);
}


/******************************************
 *                                        *
 * GObject virtual methods implementation *
 *                                        *
 ******************************************/
static void
_dispose (GObject * object)
{
  GList *tmp;
  GESContainer *self = GES_CONTAINER (object);
  GList *children;

  _ges_container_sort_children (self);
  children = ges_container_get_children (self, FALSE);

  for (tmp = g_list_last (children); tmp; tmp = tmp->prev)
    ges_container_remove (self, tmp->data);

  g_list_free_full (children, gst_object_unref);
  self->children = NULL;

  G_OBJECT_CLASS (ges_container_parent_class)->dispose (object);
}

static void
_finalize (GObject * object)
{
  GESContainer *self = GES_CONTAINER (object);

  g_list_free_full (self->priv->copied_children,
      (GDestroyNotify) _free_mapping);

  if (self->priv->mappings)
    g_hash_table_destroy (self->priv->mappings);

  G_OBJECT_CLASS (ges_container_parent_class)->finalize (object);
}

static void
_get_property (GObject * container, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GESContainer *tobj = GES_CONTAINER (container);

  switch (property_id) {
    case PROP_HEIGHT:
      g_value_set_uint (value, tobj->height);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (container, property_id, pspec);
  }
}

static void
_set_property (GObject * container, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (container, property_id, pspec);
  }
}

static void
ges_container_class_init (GESContainerClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GESTimelineElementClass *element_class = GES_TIMELINE_ELEMENT_CLASS (klass);

  GST_DEBUG_CATEGORY_INIT (ges_container_debug, "gescontainer",
      GST_DEBUG_FG_YELLOW, "ges container");

  object_class->get_property = _get_property;
  object_class->set_property = _set_property;
  object_class->dispose = _dispose;
  object_class->finalize = _finalize;

  /**
   * GESContainer:height:
   *
   * The span of priorities which this container occupies.
   */
  properties[PROP_HEIGHT] = g_param_spec_uint ("height", "Height",
      "The span of priorities this container occupies", 0, G_MAXUINT, 1,
      G_PARAM_READABLE);
  g_object_class_install_property (object_class, PROP_HEIGHT,
      properties[PROP_HEIGHT]);

  /**
   * GESContainer::child-added:
   * @container: the #GESContainer
   * @element: the #GESTimelineElement that was added.
   *
   * Will be emitted after a child was added to @container.
   * Usually you should connect with #g_signal_connect_after
   * as in the first emission stage, the signal emission might
   * get stopped internally.
   */
  ges_container_signals[CHILD_ADDED_SIGNAL] =
      g_signal_new ("child-added", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_FIRST, G_STRUCT_OFFSET (GESContainerClass, child_added),
      NULL, NULL, g_cclosure_marshal_generic,
      G_TYPE_NONE, 1, GES_TYPE_TIMELINE_ELEMENT);

  /**
   * GESContainer::child-removed:
   * @container: the #GESContainer
   * @element: the #GESTimelineElement that was removed.
   *
   * Will be emitted after a child was removed from @container.
   */
  ges_container_signals[CHILD_REMOVED_SIGNAL] =
      g_signal_new ("child-removed", G_TYPE_FROM_CLASS (klass),
      G_SIGNAL_RUN_LAST, G_STRUCT_OFFSET (GESContainerClass, child_removed),
      NULL, NULL, g_cclosure_marshal_generic, G_TYPE_NONE, 1,
      GES_TYPE_TIMELINE_ELEMENT);


  element_class->set_start = _set_start;
  element_class->set_duration = _set_duration;
  element_class->set_inpoint = _set_inpoint;
  element_class->list_children_properties = _list_children_properties;
  element_class->lookup_child = _lookup_child;
  element_class->get_track_types = _get_track_types;
  element_class->paste = _paste;
  element_class->deep_copy = _deep_copy;

  /* No default implementations */
  klass->remove_child = NULL;
  klass->add_child = NULL;
  klass->ungroup = NULL;
  klass->group = NULL;
  klass->grouping_priority = 0;
  klass->edit = NULL;
}

static void
ges_container_init (GESContainer * self)
{
  self->priv = ges_container_get_instance_private (self);

  /* FIXME, check why default was GST_SECOND? (before the existend of
   * ges-container)
   *
   * _DURATION (self) = GST_SECOND; */
  self->height = 1;             /* FIXME Why 1 and not 0? */
  self->children = NULL;

  self->priv->mappings = g_hash_table_new_full (g_direct_hash, g_direct_equal,
      NULL, (GDestroyNotify) _free_mapping);
}

/**********************************************
 *                                            *
 *    Property notifications from Children    *
 *                                            *
 **********************************************/
static void
_child_start_changed_cb (GESTimelineElement * child,
    GParamSpec * arg G_GNUC_UNUSED, GESContainer * container)
{
  ChildMapping *map;
  GstClockTime start;

  GESContainerPrivate *priv = container->priv;
  GESTimelineElement *element = GES_TIMELINE_ELEMENT (container);
  GESChildrenControlMode pmode = container->children_control_mode;

  map = g_hash_table_lookup (priv->mappings, child);
  g_assert (map);

  if (ELEMENT_FLAG_IS_SET (child, GES_TIMELINE_ELEMENT_SET_SIMPLE))
    container->children_control_mode = GES_CHILDREN_UPDATE_ALL_VALUES;

  switch (container->children_control_mode) {
    case GES_CHILDREN_IGNORE_NOTIFIES:
      return;
    case GES_CHILDREN_UPDATE_ALL_VALUES:
      _ges_container_sort_children (container);
      start = container->children ?
          _START (container->children->data) : _START (container);

      if (start != _START (container)) {
        _DURATION (container) = _END (container) - start;
        _START (container) = start;

        GST_DEBUG_OBJECT (container, "Child move made us move %" GES_FORMAT,
            GES_ARGS (container));

        g_object_notify (G_OBJECT (container), "start");
      }

      /* Falltrough! */
    case GES_CHILDREN_UPDATE_OFFSETS:
      map->start_offset = _START (container) - _START (child);
      break;

    case GES_CHILDREN_UPDATE:
      /* We update all the children calling our set_start method */
      container->initiated_move = child;
      _set_start0 (element, _START (child) + map->start_offset);
      container->initiated_move = NULL;
      break;
    default:
      break;
  }

  if (ELEMENT_FLAG_IS_SET (child, GES_TIMELINE_ELEMENT_SET_SIMPLE))
    container->children_control_mode = pmode;
}

static void
_child_inpoint_changed_cb (GESTimelineElement * child,
    GParamSpec * arg G_GNUC_UNUSED, GESContainer * container)
{
  ChildMapping *map;

  GESContainerPrivate *priv = container->priv;
  GESTimelineElement *element = GES_TIMELINE_ELEMENT (container);

  if (container->children_control_mode == GES_CHILDREN_IGNORE_NOTIFIES)
    return;

  map = g_hash_table_lookup (priv->mappings, child);
  g_assert (map);

  if (container->children_control_mode == GES_CHILDREN_UPDATE_OFFSETS
      || ELEMENT_FLAG_IS_SET (child, GES_TIMELINE_ELEMENT_SET_SIMPLE)) {
    map->inpoint_offset = _START (container) - _START (child);

    return;
  }

  /* We update all the children calling our set_inpoint method */
  container->initiated_move = child;
  _set_inpoint0 (element, _INPOINT (child) + map->inpoint_offset);
  container->initiated_move = NULL;
}

static void
_child_duration_changed_cb (GESTimelineElement * child,
    GParamSpec * arg G_GNUC_UNUSED, GESContainer * container)
{
  ChildMapping *map;

  GList *tmp;
  GstClockTime end = 0;
  GESContainerPrivate *priv = container->priv;
  GESTimelineElement *element = GES_TIMELINE_ELEMENT (container);
  GESChildrenControlMode pmode = container->children_control_mode;

  if (container->children_control_mode == GES_CHILDREN_IGNORE_NOTIFIES)
    return;

  if (ELEMENT_FLAG_IS_SET (child, GES_TIMELINE_ELEMENT_SET_SIMPLE))
    container->children_control_mode = GES_CHILDREN_UPDATE_ALL_VALUES;

  map = g_hash_table_lookup (priv->mappings, child);
  g_assert (map);

  switch (container->children_control_mode) {
    case GES_CHILDREN_IGNORE_NOTIFIES:
      break;
    case GES_CHILDREN_UPDATE_ALL_VALUES:
      _ges_container_sort_children_by_end (container);

      for (tmp = container->children; tmp; tmp = tmp->next)
        end = MAX (end, _END (tmp->data));

      if (end != _END (container)) {
        _DURATION (container) = end - _START (container);
        g_object_notify (G_OBJECT (container), "duration");
      }
      /* Falltrough */
    case GES_CHILDREN_UPDATE_OFFSETS:
      map->inpoint_offset = _START (container) - _START (child);
      break;
    case GES_CHILDREN_UPDATE:
      /* We update all the children calling our set_duration method */
      container->initiated_move = child;
      _set_duration0 (element, _DURATION (child) + map->duration_offset);
      container->initiated_move = NULL;
      break;
    default:
      break;
  }

  if (ELEMENT_FLAG_IS_SET (child, GES_TIMELINE_ELEMENT_SET_SIMPLE))
    container->children_control_mode = pmode;
}

/****************************************************
 *                                                  *
 *         Internal methods implementation          *
 *                                                  *
 ****************************************************/

void
_ges_container_sort_children (GESContainer * container)
{
  container->children = g_list_sort (container->children,
      (GCompareFunc) element_start_compare);
}

void
_ges_container_sort_children_by_end (GESContainer * container)
{
  container->children = g_list_sort (container->children,
      (GCompareFunc) element_end_compare);
}

void
_ges_container_set_height (GESContainer * container, guint32 height)
{
  if (container->height != height) {
    container->height = height;
    GST_DEBUG_OBJECT (container, "Updating height %i", container->height);
    g_object_notify (G_OBJECT (container), "height");
  }
}

gint
_ges_container_get_priority_offset (GESContainer * container,
    GESTimelineElement * elem)
{
  ChildMapping *map = g_hash_table_lookup (container->priv->mappings, elem);

  g_return_val_if_fail (map, 0);

  return map->priority_offset;
}

void
_ges_container_set_priority_offset (GESContainer * container,
    GESTimelineElement * elem, gint32 priority_offset)
{
  ChildMapping *map = g_hash_table_lookup (container->priv->mappings, elem);

  g_return_if_fail (map);

  map->priority_offset = priority_offset;
}

/**********************************************
 *                                            *
 *            API implementation              *
 *                                            *
 **********************************************/

/**
 * ges_container_add:
 * @container: a #GESContainer
 * @child: the #GESTimelineElement
 *
 * Add the #GESTimelineElement to the container.
 *
 * Returns: %TRUE on success, %FALSE on failure.
 */
gboolean
ges_container_add (GESContainer * container, GESTimelineElement * child)
{
  ChildMapping *mapping;
  gboolean notify_start = FALSE;
  GESContainerClass *class;
  GESContainerPrivate *priv;

  g_return_val_if_fail (GES_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (GES_IS_TIMELINE_ELEMENT (child), FALSE);
  g_return_val_if_fail (GES_TIMELINE_ELEMENT_PARENT (child) == NULL, FALSE);

  class = GES_CONTAINER_GET_CLASS (container);
  priv = container->priv;

  GST_DEBUG_OBJECT (container, "adding timeline element %" GST_PTR_FORMAT,
      child);

  container->children_control_mode = GES_CHILDREN_IGNORE_NOTIFIES;
  if (class->add_child) {
    if (class->add_child (container, child) == FALSE) {
      container->children_control_mode = GES_CHILDREN_UPDATE;
      GST_WARNING_OBJECT (container, "Erreur adding child %p", child);
      return FALSE;
    }
  }
  container->children_control_mode = GES_CHILDREN_UPDATE;

  if (_START (container) > _START (child)) {
    _START (container) = _START (child);

    g_hash_table_foreach (priv->mappings, (GHFunc) _resync_start_offsets,
        container);
    notify_start = TRUE;
  }

  mapping = g_slice_new0 (ChildMapping);
  mapping->child = gst_object_ref (child);
  mapping->start_offset = _START (container) - _START (child);
  mapping->duration_offset = _DURATION (container) - _DURATION (child);
  mapping->inpoint_offset = _INPOINT (container) - _INPOINT (child);

  g_hash_table_insert (priv->mappings, child, mapping);

  container->children = g_list_prepend (container->children, child);

  _ges_container_sort_children (container);

  /* Listen to all property changes */
  mapping->start_notifyid =
      g_signal_connect (G_OBJECT (child), "notify::start",
      G_CALLBACK (_child_start_changed_cb), container);
  mapping->duration_notifyid =
      g_signal_connect (G_OBJECT (child), "notify::duration",
      G_CALLBACK (_child_duration_changed_cb), container);
  mapping->inpoint_notifyid =
      g_signal_connect (G_OBJECT (child), "notify::in-point",
      G_CALLBACK (_child_inpoint_changed_cb), container);

  if (ges_timeline_element_set_parent (child, GES_TIMELINE_ELEMENT (container))
      == FALSE) {
    if (class->remove_child)
      class->remove_child (container, child);

    g_hash_table_remove (priv->mappings, child);
    container->children = g_list_remove (container->children, child);
    _ges_container_sort_children (container);

    return FALSE;
  }

  _ges_container_add_child_properties (container, child);

  priv->adding_children = g_list_prepend (priv->adding_children, child);
  g_signal_emit (container, ges_container_signals[CHILD_ADDED_SIGNAL], 0,
      child);
  priv->adding_children = g_list_remove (priv->adding_children, child);

  if (notify_start)
    g_object_notify (G_OBJECT (container), "start");

  return TRUE;
}

/**
 * ges_container_remove:
 * @container: a #GESContainer
 * @child: the #GESTimelineElement to release
 *
 * Release the @child from the control of @container.
 *
 * Returns: %TRUE if the @child was properly released, else %FALSE.
 */
gboolean
ges_container_remove (GESContainer * container, GESTimelineElement * child)
{
  GESContainerClass *klass;
  GESContainerPrivate *priv;

  g_return_val_if_fail (GES_IS_CONTAINER (container), FALSE);
  g_return_val_if_fail (GES_IS_TIMELINE_ELEMENT (child), FALSE);

  GST_DEBUG_OBJECT (container, "removing child: %" GST_PTR_FORMAT, child);

  klass = GES_CONTAINER_GET_CLASS (container);
  priv = container->priv;

  if (!(g_hash_table_lookup (priv->mappings, child))) {
    GST_WARNING_OBJECT (container, "Element isn't controlled by this "
        "container");
    return FALSE;
  }

  if (klass->remove_child) {
    if (klass->remove_child (container, child) == FALSE)
      return FALSE;
  }

  container->children = g_list_remove (container->children, child);
  /* Let it live removing from our mappings */
  g_hash_table_remove (priv->mappings, child);

  _ges_container_remove_child_properties (container, child);

  if (!g_list_find (container->priv->adding_children, child)) {
    g_signal_emit (container, ges_container_signals[CHILD_REMOVED_SIGNAL], 0,
        child);
  } else {
    GST_INFO_OBJECT (container, "Not emitting 'child-removed' signal as child"
        " removal happend during 'child-added' signal emission");
  }
  gst_object_unref (child);

  return TRUE;
}

static void
_get_children_recursively (GESContainer * container, GList ** children)
{
  GList *tmp;

  *children =
      g_list_concat (*children, g_list_copy_deep (container->children,
          (GCopyFunc) gst_object_ref, NULL));

  for (tmp = container->children; tmp; tmp = tmp->next) {
    GESTimelineElement *element = tmp->data;

    if (GES_IS_CONTAINER (element))
      _get_children_recursively (tmp->data, children);
  }
}

/**
 * ges_container_get_children:
 * @container: a #GESContainer
 * @recursive:  Whether to recursively get children in @container
 *
 * Get the list of #GESTimelineElement contained in @container
 * The user is responsible for unreffing the contained objects
 * and freeing the list.
 *
 * Returns: (transfer full) (element-type GESTimelineElement): The list of
 * timeline element contained in @container.
 */
GList *
ges_container_get_children (GESContainer * container, gboolean recursive)
{
  GList *children = NULL;

  g_return_val_if_fail (GES_IS_CONTAINER (container), NULL);

  if (!recursive)
    return g_list_copy_deep (container->children, (GCopyFunc) gst_object_ref,
        NULL);

  _get_children_recursively (container, &children);
  return children;
}

/**
 * ges_container_ungroup:
 * @container: (transfer full): The #GESContainer to ungroup
 * @recursive: Wether to recursively ungroup @container
 *
 * Ungroups the #GESTimelineElement contained in this GESContainer,
 * creating new #GESContainer containing those #GESTimelineElement
 * apropriately.
 *
 * Returns: (transfer full) (element-type GESContainer): The list of
 * #GESContainer resulting from the ungrouping operation
 * The user is responsible for unreffing the contained objects
 * and freeing the list.
 */
GList *
ges_container_ungroup (GESContainer * container, gboolean recursive)
{
  GESContainerClass *klass;

  g_return_val_if_fail (GES_IS_CONTAINER (container), NULL);

  GST_DEBUG_OBJECT (container, "Ungrouping container %s recursively",
      recursive ? "" : "not");

  klass = GES_CONTAINER_GET_CLASS (container);
  if (klass->ungroup == NULL) {
    GST_INFO_OBJECT (container, "No ungoup virtual method, doint nothing");
    return NULL;
  }

  return klass->ungroup (container, recursive);
}

/**
 * ges_container_group:
 * @containers: (transfer none)(element-type GESContainer) (allow-none): The
 * #GESContainer to group, they must all be in a same #GESTimeline
 *
 * Groups the #GESContainer-s provided in @containers. It creates a subclass
 * of #GESContainer, depending on the containers provided in @containers.
 * Basically, if all the containers in @containers should be contained in a same
 * clip (all the #GESTrackElement they contain have the exact same
 * start/inpoint/duration and are in the same layer), it will create a #GESClip
 * otherwise a #GESGroup will be created
 *
 * Returns: (transfer none): The #GESContainer (subclass) resulting of the
 * grouping
 */
GESContainer *
ges_container_group (GList * containers)
{
  GList *tmp;
  guint n_children;
  GType *children_types;
  GESTimelineElement *element;
  GObjectClass *clip_class;

  guint i = 0;
  GESContainer *ret = NULL;
  GESTimeline *timeline = NULL;

  if (containers) {
    element = GES_TIMELINE_ELEMENT (containers->data);
    timeline = GES_TIMELINE_ELEMENT_TIMELINE (element);
    g_return_val_if_fail (timeline, NULL);
  }

  if (g_list_length (containers) == 1)
    return containers->data;

  for (tmp = containers; tmp; tmp = tmp->next) {
    g_return_val_if_fail (GES_IS_CONTAINER (tmp->data), NULL);
    g_return_val_if_fail (GES_TIMELINE_ELEMENT_PARENT (tmp->data) == NULL,
        NULL);
    g_return_val_if_fail (GES_TIMELINE_ELEMENT_TIMELINE (tmp->data) == timeline,
        NULL);
  }

  children_types = g_type_children (GES_TYPE_CONTAINER, &n_children);
  g_qsort_with_data (children_types, n_children, sizeof (GType),
      (GCompareDataFunc) compare_grouping_prio, NULL);

  for (i = 0; i < n_children; i++) {
    clip_class = g_type_class_peek (children_types[i]);
    ret = GES_CONTAINER_CLASS (clip_class)->group (containers);

    if (ret)
      break;
  }

  g_free (children_types);
  return ret;
}

/**
 * ges_container_edit:
 * @container: the #GESClip to edit
 * @layers: (element-type GESLayer): The layers you want the edit to
 *  happen in, %NULL means that the edition is done in all the
 *  #GESLayers contained in the current timeline.
 * @new_layer_priority: The priority of the layer @container should land in.
 *  If the layer you're trying to move the container to doesn't exist, it will
 *  be created automatically. -1 means no move.
 * @mode: The #GESEditMode in which the editition will happen.
 * @edge: The #GESEdge the edit should happen on.
 * @position: The position at which to edit @container (in nanosecond)
 *
 * Edit @container in the different exisiting #GESEditMode modes. In the case of
 * slide, and roll, you need to specify a #GESEdge
 *
 * Returns: %TRUE if the container as been edited properly, %FALSE if an error
 * occured
 *
 * Deprecated: 1.18: use #ges_timeline_element_edit instead.
 */
gboolean
ges_container_edit (GESContainer * container, GList * layers,
    gint new_layer_priority, GESEditMode mode, GESEdge edge, guint64 position)
{
  g_return_val_if_fail (GES_IS_CONTAINER (container), FALSE);

  return ges_timeline_element_edit (GES_TIMELINE_ELEMENT (container),
      layers, new_layer_priority, mode, edge, position);
}
