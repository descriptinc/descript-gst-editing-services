/* GStreamer Editing Services
 * Copyright (C) 2009 Edward Hervey <edward.hervey@collabora.co.uk>
 *               2009 Nokia Corporation
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

#ifndef __GES_INTERNAL_H__
#define __GES_INTERNAL_H__
#include <gst/gst.h>
#include <gst/pbutils/encoding-profile.h>
#include <gio/gio.h>

#include "ges-timeline.h"
#include "ges-track-element.h"
#include "ges-timeline-element.h"

#include "ges-asset.h"
#include "ges-base-xml-formatter.h"
#include "ges-timeline-tree.h"

G_BEGIN_DECLS

GST_DEBUG_CATEGORY_EXTERN (_ges_debug);
#ifndef GST_CAT_DEFAULT
#define GST_CAT_DEFAULT _ges_debug
#endif

/*  The first 2 NLE priorities are used for:
 *    0- The Mixing element
 *    1- The Gaps
 */
#define MIN_NLE_PRIO 2
#define LAYER_HEIGHT 1000

#define _START(obj) GES_TIMELINE_ELEMENT_START (obj)
#define _INPOINT(obj) GES_TIMELINE_ELEMENT_INPOINT (obj)
#define _DURATION(obj) GES_TIMELINE_ELEMENT_DURATION (obj)
#define _MAXDURATION(obj) GES_TIMELINE_ELEMENT_MAX_DURATION (obj)
#define _PRIORITY(obj) GES_TIMELINE_ELEMENT_PRIORITY (obj)
#ifndef _END
#define _END(obj) (_START (obj) + _DURATION (obj))
#endif
#define _set_start0 ges_timeline_element_set_start
#define _set_inpoint0 ges_timeline_element_set_inpoint
#define _set_duration0 ges_timeline_element_set_duration
#define _set_priority0 ges_timeline_element_set_priority

#define GES_TIMELINE_ELEMENT_FORMAT \
    "s<%p>" \
    " [ %" GST_TIME_FORMAT \
    " (%" GST_TIME_FORMAT \
    ") - %" GST_TIME_FORMAT "(%" GST_TIME_FORMAT") layer: %" G_GINT32_FORMAT "] "

#define GES_TIMELINE_ELEMENT_ARGS(element) \
    GES_TIMELINE_ELEMENT_NAME(element), element, \
    GST_TIME_ARGS(GES_TIMELINE_ELEMENT_START(element)), \
    GST_TIME_ARGS(GES_TIMELINE_ELEMENT_INPOINT(element)), \
    GST_TIME_ARGS(GES_TIMELINE_ELEMENT_DURATION(element)), \
    GST_TIME_ARGS(GES_TIMELINE_ELEMENT_MAX_DURATION(element)), \
    GES_TIMELINE_ELEMENT_LAYER_PRIORITY(element)

#define GES_FORMAT GES_TIMELINE_ELEMENT_FORMAT
#define GES_ARGS GES_TIMELINE_ELEMENT_ARGS

G_GNUC_INTERNAL gboolean
timeline_ripple_object         (GESTimeline *timeline, GESTimelineElement *obj,
                                gint new_layer_priority,
                                GList * layers, GESEdge edge,
                                guint64 position);

G_GNUC_INTERNAL gboolean
timeline_slide_object          (GESTimeline *timeline, GESTrackElement *obj,
                                    GList * layers, GESEdge edge, guint64 position);

G_GNUC_INTERNAL gboolean
timeline_roll_object           (GESTimeline *timeline, GESTimelineElement *obj,
                                GList * layers, GESEdge edge, guint64 position);

G_GNUC_INTERNAL gboolean
timeline_trim_object           (GESTimeline *timeline, GESTimelineElement * object,
                                guint32 new_layer_priority, GList * layers, GESEdge edge,
                                guint64 position);
G_GNUC_INTERNAL gboolean
ges_timeline_trim_object_simple (GESTimeline * timeline, GESTimelineElement * obj,
                                 guint32 new_layer_priority, GList * layers, GESEdge edge,
                                 guint64 position, gboolean snapping);

G_GNUC_INTERNAL gboolean
ges_timeline_move_object_simple (GESTimeline * timeline, GESTimelineElement * object,
                                 GList * layers, GESEdge edge, guint64 position);

G_GNUC_INTERNAL gboolean
timeline_move_object           (GESTimeline *timeline, GESTimelineElement * object,
                                guint32 new_layer_priority, GList * layers, GESEdge edge,
                                guint64 position);

G_GNUC_INTERNAL void
timeline_add_group             (GESTimeline *timeline,
                                GESGroup *group);
G_GNUC_INTERNAL void
timeline_remove_group          (GESTimeline *timeline,
                                GESGroup *group);
G_GNUC_INTERNAL void
timeline_emit_group_added      (GESTimeline *timeline,
                                GESGroup *group);
G_GNUC_INTERNAL void
timeline_emit_group_removed    (GESTimeline * timeline,
                                GESGroup * group, GPtrArray * array);

G_GNUC_INTERNAL
gboolean
timeline_add_element           (GESTimeline *timeline,
                                GESTimelineElement *element);
G_GNUC_INTERNAL
gboolean
timeline_remove_element       (GESTimeline *timeline,
                               GESTimelineElement *element);

G_GNUC_INTERNAL
GNode *
timeline_get_tree           (GESTimeline *timeline);

G_GNUC_INTERNAL
void
timeline_update_transition (GESTimeline *timeline);

G_GNUC_INTERNAL
void
timeline_fill_gaps            (GESTimeline *timeline);

G_GNUC_INTERNAL void
timeline_create_transitions (GESTimeline * timeline, GESTrackElement * track_element);

G_GNUC_INTERNAL
void
track_resort_and_fill_gaps    (GESTrack *track);

G_GNUC_INTERNAL
void
track_disable_last_gap        (GESTrack *track, gboolean disabled);

G_GNUC_INTERNAL void
ges_asset_cache_init (void);

G_GNUC_INTERNAL void
ges_asset_cache_deinit (void);

G_GNUC_INTERNAL void
ges_asset_set_id (GESAsset *asset, const gchar *id);

G_GNUC_INTERNAL void
ges_asset_cache_put (GESAsset * asset, GTask *task);

G_GNUC_INTERNAL gboolean
ges_asset_cache_set_loaded(GType extractable_type, const gchar * id, GError *error);

GES_API GESAsset*
ges_asset_cache_lookup(GType extractable_type, const gchar * id);

GES_API gboolean
ges_asset_try_proxy (GESAsset *asset, const gchar *new_id);

G_GNUC_INTERNAL gboolean
ges_asset_request_id_update (GESAsset *asset, gchar **proposed_id,
    GError *error);
G_GNUC_INTERNAL gchar *
ges_effect_assect_id_get_type_and_bindesc (const char    *id,
                                           GESTrackType  *track_type,
                                           GError       **error);

G_GNUC_INTERNAL void _ges_uri_asset_cleanup (void);

G_GNUC_INTERNAL gboolean _ges_uri_asset_ensure_setup (gpointer uriasset_class);

/* GESExtractable internall methods
 *
 * FIXME Check if that should be public later
 */
G_GNUC_INTERNAL GType
ges_extractable_type_get_asset_type              (GType type);

G_GNUC_INTERNAL gchar *
ges_extractable_type_check_id                    (GType type, const gchar *id, GError **error);

G_GNUC_INTERNAL GParameter *
ges_extractable_type_get_parameters_from_id      (GType type, const gchar *id,
                                                  guint *n_params);
G_GNUC_INTERNAL GType
ges_extractable_get_real_extractable_type_for_id (GType type, const gchar * id);

G_GNUC_INTERNAL gboolean
ges_extractable_register_metas                   (GType extractable_type, GESAsset *asset);

/************************************************
 *                                              *
 *        GESFormatter internal methods         *
 *                                              *
 ************************************************/
G_GNUC_INTERNAL void
ges_formatter_set_project                        (GESFormatter *formatter,
                                                  GESProject *project);
G_GNUC_INTERNAL GESProject *
ges_formatter_get_project                        (GESFormatter *formatter);
G_GNUC_INTERNAL  GESAsset *
_find_formatter_asset_for_id                     (const gchar *id);



/************************************************
 *                                              *
 *        GESProject internal methods           *
 *                                              *
 ************************************************/

/* FIXME This should probably become public, but we need to make sure it
 * is the right API before doing so */
G_GNUC_INTERNAL  gboolean ges_project_set_loaded                  (GESProject * project,
                                                                   GESFormatter *formatter);
G_GNUC_INTERNAL  gchar * ges_project_try_updating_id              (GESProject *self,
                                                                   GESAsset *asset,
                                                                   GError *error);
G_GNUC_INTERNAL  void ges_project_add_loading_asset               (GESProject *project,
                                                                   GType extractable_type,
                                                                   const gchar *id);
G_GNUC_INTERNAL  gchar* ges_uri_asset_try_update_id               (GError *error, GESAsset *wrong_asset);
/************************************************
 *                                              *
 *   GESBaseXmlFormatter internal methods       *
 *                                              *
 ************************************************/

/* FIXME GESBaseXmlFormatter is all internal for now, the API is not stable
 * fo now, so do not expose it */
G_GNUC_INTERNAL void ges_base_xml_formatter_add_clip (GESBaseXmlFormatter * self,
                                                                 const gchar *id,
                                                                 const char *asset_id,
                                                                 GType type,
                                                                 GstClockTime start,
                                                                 GstClockTime inpoint,
                                                                 GstClockTime duration,
                                                                 guint layer_prio,
                                                                 GESTrackType track_types,
                                                                 GstStructure *properties,
                                                                 GstStructure * children_properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_asset        (GESBaseXmlFormatter * self,
                                                                 const gchar * id,
                                                                 GType extractable_type,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 const gchar *proxy_id,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_layer           (GESBaseXmlFormatter *self,
                                                                 GType extractable_type,
                                                                 guint priority,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_track           (GESBaseXmlFormatter *self,
                                                                 GESTrackType track_type,
                                                                 GstCaps *caps,
                                                                 const gchar *id,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_encoding_profile(GESBaseXmlFormatter * self,
                                                                 const gchar *type,
                                                                 const gchar *parent,
                                                                 const gchar * name,
                                                                 const gchar * description,
                                                                 GstCaps * format,
                                                                 const gchar * preset,
                                                                 GstStructure * preset_properties,
                                                                 const gchar * preset_name,
                                                                 guint id,
                                                                 guint presence,
                                                                 GstCaps * restriction,
                                                                 guint pass,
                                                                 gboolean variableframerate,
                                                                 GstStructure * properties,
                                                                 gboolean enabled,
                                                                 GError ** error);
G_GNUC_INTERNAL void ges_base_xml_formatter_add_track_element   (GESBaseXmlFormatter *self,
                                                                 GType effect_type,
                                                                 const gchar *asset_id,
                                                                 const gchar * track_id,
                                                                 const gchar *timeline_obj_id,
                                                                 GstStructure *children_properties,
                                                                 GstStructure *properties,
                                                                 const gchar *metadatas,
                                                                 GError **error);

G_GNUC_INTERNAL void ges_base_xml_formatter_add_source          (GESBaseXmlFormatter *self,
                                                                 const gchar * track_id,
                                                                 GstStructure *children_properties);

G_GNUC_INTERNAL void ges_base_xml_formatter_add_group           (GESBaseXmlFormatter *self,
                                                                 const gchar *name,
                                                                 const gchar *properties,
                                                                 const gchar *metadatas);

G_GNUC_INTERNAL void ges_base_xml_formatter_last_group_add_child(GESBaseXmlFormatter *self,
                                                                 const gchar * id,
                                                                 const gchar * name);

G_GNUC_INTERNAL void ges_base_xml_formatter_add_control_binding (GESBaseXmlFormatter * self,
                                                                  const gchar * binding_type,
                                                                  const gchar * source_type,
                                                                  const gchar * property_name,
                                                                  gint mode,
                                                                  const gchar *track_id,
                                                                  GSList * timed_values);

G_GNUC_INTERNAL gboolean set_property_foreach                   (GQuark field_id,
                                                                 const GValue * value,
                                                                 GObject * object);

G_GNUC_INTERNAL GstElement * get_element_for_encoding_profile   (GstEncodingProfile *prof,
                                                                 GstElementFactoryListType type);

/* Function to initialise GES */
G_GNUC_INTERNAL void _init_standard_transition_assets        (void);
G_GNUC_INTERNAL void _init_formatter_assets                  (void);

/* Utilities */
G_GNUC_INTERNAL gint element_start_compare                (GESTimelineElement * a,
                                                           GESTimelineElement * b);
G_GNUC_INTERNAL gint element_end_compare                  (GESTimelineElement * a,
                                                           GESTimelineElement * b);
G_GNUC_INTERNAL GstElementFactory *
ges_get_compositor_factory                                (void);

G_GNUC_INTERNAL void
ges_base_xml_formatter_set_timeline_properties(GESBaseXmlFormatter * self,
					       GESTimeline *timeline,
					       const gchar *properties,
					       const gchar *metadatas);

/****************************************************
 *              GESContainer                        *
 ****************************************************/
G_GNUC_INTERNAL void _ges_container_sort_children         (GESContainer *container);
G_GNUC_INTERNAL void _ges_container_sort_children_by_end  (GESContainer *container);
G_GNUC_INTERNAL void _ges_container_set_height            (GESContainer * container,
                                                           guint32 height);
G_GNUC_INTERNAL gint  _ges_container_get_priority_offset  (GESContainer * container,
                                                           GESTimelineElement *elem);
G_GNUC_INTERNAL void _ges_container_set_priority_offset   (GESContainer * container,
                                                           GESTimelineElement *elem,
                                                           gint32 priority_offset);


/****************************************************
 *                  GESClip                         *
 ****************************************************/
G_GNUC_INTERNAL void              ges_clip_set_layer              (GESClip *clip, GESLayer  *layer);
G_GNUC_INTERNAL gboolean          ges_clip_is_moving_from_layer   (GESClip *clip);
G_GNUC_INTERNAL void              ges_clip_set_moving_from_layer  (GESClip *clip, gboolean is_moving);
G_GNUC_INTERNAL GESTrackElement*  ges_clip_create_track_element   (GESClip *clip, GESTrackType type);
G_GNUC_INTERNAL GList*            ges_clip_create_track_elements  (GESClip *clip, GESTrackType type);

/****************************************************
 *              GESLayer                            *
 ****************************************************/
G_GNUC_INTERNAL gboolean ges_layer_resync_priorities (GESLayer * layer);
G_GNUC_INTERNAL void layer_set_priority               (GESLayer * layer, guint priority, gboolean emit);

/****************************************************
 *              GESTrackElement                     *
 ****************************************************/
#define         NLE_OBJECT_TRACK_ELEMENT_QUARK                  (g_quark_from_string ("nle_object_track_element_quark"))
G_GNUC_INTERNAL gboolean  ges_track_element_set_track           (GESTrackElement * object, GESTrack * track);
G_GNUC_INTERNAL void ges_track_element_copy_properties          (GESTimelineElement * element,
                                                                 GESTimelineElement * elementcopy);

G_GNUC_INTERNAL void ges_track_element_copy_bindings (GESTrackElement *element,
						       GESTrackElement *new_element,
						       guint64 position);

G_GNUC_INTERNAL GstElement *ges_source_create_topbin (const gchar * bin_name, GstElement * sub_element, ...);
G_GNUC_INTERNAL void ges_track_set_caps                (GESTrack *track,
                                                        const GstCaps *caps);
G_GNUC_INTERNAL GstElement * ges_track_get_composition (GESTrack *track);


/*********************************************
 *  GESTrackElement subclasses contructores  *
 ********************************************/
G_GNUC_INTERNAL GESAudioTestSource * ges_audio_test_source_new (void);
G_GNUC_INTERNAL GESAudioUriSource  * ges_audio_uri_source_new  (gchar *uri);
G_GNUC_INTERNAL GESVideoUriSource  * ges_video_uri_source_new  (gchar *uri);
G_GNUC_INTERNAL GESImageSource     * ges_image_source_new      (gchar *uri);
G_GNUC_INTERNAL GESTitleSource     * ges_title_source_new      (void);
G_GNUC_INTERNAL GESVideoTestSource * ges_video_test_source_new (void);

/****************************************************
 *              GESTimelineElement                  *
 ****************************************************/
typedef enum
{
  GES_CLIP_IS_MOVING = (1 << 0),
  GES_TIMELINE_ELEMENT_SET_SIMPLE = (1 << 1),
} GESTimelineElementFlags;

G_GNUC_INTERNAL gdouble ges_timeline_element_get_media_duration_factor(GESTimelineElement *self);
G_GNUC_INTERNAL GESTimelineElement * ges_timeline_element_get_copied_from (GESTimelineElement *self);
G_GNUC_INTERNAL GESTimelineElementFlags ges_timeline_element_flags (GESTimelineElement *self);
G_GNUC_INTERNAL void                ges_timeline_element_set_flags (GESTimelineElement *self, GESTimelineElementFlags flags);

#define ELEMENT_FLAGS(obj)             (ges_timeline_element_flags (GES_TIMELINE_ELEMENT(obj)))
#define ELEMENT_SET_FLAG(obj,flag)     (ges_timeline_element_set_flags(GES_TIMELINE_ELEMENT(obj), (ELEMENT_FLAGS(obj) | (flag))))
#define ELEMENT_UNSET_FLAG(obj,flag)   (ges_timeline_element_set_flags(GES_TIMELINE_ELEMENT(obj), (ELEMENT_FLAGS(obj) & ~(flag))))
#define ELEMENT_FLAG_IS_SET(obj,flag)  ((ELEMENT_FLAGS (obj) & (flag)) == (flag))

/******************************
 *  GESMultiFile internal API *
 ******************************/
typedef struct GESMultiFileURI
{
  gchar *location;
  gint start;
  gint end;
} GESMultiFileURI;

G_GNUC_INTERNAL GESMultiFileURI * ges_multi_file_uri_new (const gchar * uri);

/************************
 * Our property masks   *
 ************************/
#define GES_PARAM_NO_SERIALIZATION (1 << (G_PARAM_USER_SHIFT + 1))

/********************
 *  Gnonlin helpers *
 ********************/

G_GNUC_INTERNAL gboolean ges_nle_composition_add_object (GstElement *comp, GstElement *object);
G_GNUC_INTERNAL gboolean ges_nle_composition_remove_object (GstElement *comp, GstElement *object);
G_GNUC_INTERNAL gboolean ges_nle_object_commit (GstElement * nlesource, gboolean recurse);

G_END_DECLS

#endif /* __GES_INTERNAL_H__ */
