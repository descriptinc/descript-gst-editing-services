/* GStreamer Editing Services GStreamer plugin
 * Copyright (C) 2019 Thibault Saunier <tsaunier@igalia.com>
 *
 * gessrc.c
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
 *

 **
 * SECTION:element-gessrc
 * @short_description: A GstBin subclasses use to use GESTimeline
 * as sources inside any GstPipeline.
 * @see_also: #GESTimeline
 *
 * The gessrc is a bin that will simply expose the track src pads
 * and implements the GstUriHandler interface using a custom `ges://`
 * uri scheme.
 *
 * NOTE: That to use it inside playbin and friends you **need** to
 * set gessrc::timeline property yourself.
 *
 * Example with #playbin:
 *
 * {{../../examples/c/gessrc.c}}
 *
 * Example with #GstPlayer:
 *
 * {{../../examples/python/gst-player.py}}
 **/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <ges/ges.h>

#include "gesbasebin.h"

GST_DEBUG_CATEGORY_STATIC (gessrc);
#define GST_CAT_DEFAULT gessrc

G_DECLARE_FINAL_TYPE (GESSrc, ges_src, GES, SRC, GESBaseBin);
struct _GESSrc
{
  GESBaseBin parent;
};
#define GES_SRC(obj) ((GESSrc*) obj)

/*** GSTURIHANDLER INTERFACE *************************************************/

static GstURIType
ges_src_uri_get_type (GType type)
{
  return GST_URI_SRC;
}

static const gchar *const *
ges_src_uri_get_protocols (GType type)
{
  static const gchar *protocols[] = { "ges", NULL };

  return protocols;
}

static gchar *
ges_src_uri_get_uri (GstURIHandler * handler)
{
  GESSrc *self = GES_SRC (handler);
  GESTimeline *timeline = ges_base_bin_get_timeline (GES_BASE_BIN (self));

  return timeline ? g_strdup_printf ("ges://%s",
      GST_OBJECT_NAME (timeline)) : NULL;
}

static gboolean
ges_src_uri_set_uri (GstURIHandler * handler, const gchar * uri,
    GError ** error)
{
  return TRUE;
}

static void
ges_src_uri_handler_init (gpointer g_iface, gpointer iface_data)
{
  GstURIHandlerInterface *iface = (GstURIHandlerInterface *) g_iface;

  iface->get_type = ges_src_uri_get_type;
  iface->get_protocols = ges_src_uri_get_protocols;
  iface->get_uri = ges_src_uri_get_uri;
  iface->set_uri = ges_src_uri_set_uri;
}

G_DEFINE_TYPE_WITH_CODE (GESSrc, ges_src, ges_base_bin_get_type (),
    G_IMPLEMENT_INTERFACE (GST_TYPE_URI_HANDLER, ges_src_uri_handler_init));

static void
ges_src_class_init (GESSrcClass * self_class)
{
  GstElementClass *gstelement_klass = GST_ELEMENT_CLASS (self_class);

  GST_DEBUG_CATEGORY_INIT (gessrc, "gessrc", 0, "ges src element");
  gst_element_class_set_static_metadata (gstelement_klass,
      "GStreamer Editing Services based 'source'",
      "Codec/Source/Editing",
      "Source for GESTimeline.", "Thibault Saunier <tsaunier@igalia.com");
}

static void
ges_src_init (GESSrc * self)
{
}
