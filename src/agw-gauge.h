#ifndef __AGW_GAUGE_H__
#define __AGW_GAUGE_H__

#include <gtk/gtk.h>


G_BEGIN_DECLS

#define AGW_TYPE_GAUGE agw_gauge_get_type()

G_DECLARE_FINAL_TYPE(AgwGauge, agw_gauge, AGW, GAUGE, GtkRange)


GtkWidget *     agw_gauge_new               (void);
gboolean        agw_gauge_set_theme         (AgwGauge *     gauge,
                                             const gchar *  theme_dir,
                                             GError **      error);
void            agw_gauge_set_value         (AgwGauge *     gauge,
                                             gdouble        value);

G_END_DECLS

#endif /* __AGW_GAUGE_H__ */
