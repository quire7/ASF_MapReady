#include "asf_convert_gui.h"
#include <gdk/gdkkeysyms.h>

static const int max_line_len = 2048;

gchar * build_asf_metadata_filename(gchar * name)
{
    gchar * p;

    p = strrchr(name, '.');
    if (!p)
    {
        return g_strdup(name);
    }
    else
    {
        gchar * ret;	
        ret = (gchar *) g_malloc( sizeof(gchar) * (strlen(name) + 5) );

        strcpy(ret, name);
        *(ret + (p - name + 1)) = '\0';

        strcat(ret, "meta");

        return ret;
    }
}

gchar * build_ceos_metadata_filename(gchar * name)
{
    gchar * p;

    p = strrchr(name, '.');
    if (!p)
    {
        return g_strdup(name);
    }
    else
    {
        gchar * ret;	
        ret = (gchar *) g_malloc( sizeof(gchar) * (strlen(name) + 5) );

        strcpy(ret, name);
        *(ret + (p - name + 1)) = '\0';

        strcat(ret, "L");

        return ret;
    }
}

//static gchar *
//change_extension(const gchar * file, const gchar * ext)
//{
//    gchar * replaced = (gchar *)
//        g_malloc(sizeof(gchar) * (strlen(file) + strlen(ext) + 10));
//
//    strcpy(replaced, file);
//    char * p = strrchr(replaced, '.');
//
//    if (p)
//        *p = '\0';
//
//    strcat(replaced, ".");
//    strcat(replaced, ext);
//
//    return replaced;
//}

void show_asf_meta_data(gchar * out_name)
{
    GtkWidget *metadata_dialog;
    GtkWidget *metadata_text;
    GtkWidget *metadata_label;
    GtkTextBuffer * text_buffer;
    FILE * metadata_file;
    gchar * metadata_filename;
    gchar * label_text;

    metadata_dialog =
        glade_xml_get_widget(glade_xml, "metadata_dialog");

    metadata_text =
        glade_xml_get_widget(glade_xml, "metadata_text");

    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(metadata_text));

    gtk_text_buffer_set_text(text_buffer, "", -1);

    metadata_filename = build_asf_metadata_filename(out_name);

    label_text = (gchar *) g_malloc(sizeof(gchar) * 
        (strlen(metadata_filename) + 32));

    metadata_file = fopen(metadata_filename, "rt");

    if (metadata_file)
    {
        gchar * buffer = (gchar *) g_malloc(sizeof(gchar) * max_line_len);
        while (!feof(metadata_file))
        {
            gchar *p = fgets(buffer, max_line_len, metadata_file);
            if (p)
            {
                GtkTextIter end;

                gtk_text_buffer_get_end_iter(text_buffer, &end);
                gtk_text_buffer_insert(text_buffer, &end, buffer, -1);
            }
        }

        fclose(metadata_file);
        g_free(buffer);

        metadata_label =
            glade_xml_get_widget(glade_xml, "metadata_label");

        sprintf(label_text, "Meta Data File: %s", metadata_filename);
        gtk_label_set_text(GTK_LABEL(metadata_label), label_text);

        gtk_widget_show(metadata_dialog);

        /* user may have selected "Display Metadata" when the meta data
        window was already opened -- bring it to the top */
        gtk_window_present(GTK_WINDOW(metadata_dialog));
    }
    else
    {
        sprintf(label_text, "Meta Data File Not Found: %s", metadata_filename);
        message_box(label_text);
    }

    g_free(label_text);
    g_free(metadata_filename);
}

#ifdef THUMBNAILS
static void mdv_thread (GString *file, gpointer user_data)
{
#ifdef win32
    gchar * mdv = find_in_path("mdv.exe");
#else
    gchar * mdv = find_in_path("mdv");
#endif

    char buf[1024];
    char *escaped_str = escapify(file->str);
    sprintf(buf, "\"%s\" \"%s\"", mdv, escaped_str);
    free(escaped_str);
    do_system_exec(buf);
    g_string_free(file, TRUE);
}
#endif

void show_ceos_meta_data(gchar * out_name)
{
#ifdef win32
    gchar * mdv = find_in_path("mdv.exe");
#else
    gchar * mdv = find_in_path("mdv");
#endif

    gchar * ceos_file = build_ceos_metadata_filename(out_name);

    // use_thumbnails should always be true here, since
    // we disable the option in the case where it is false
    if (mdv && use_thumbnails)
    {
#ifdef THUMBNAILS
        static GThreadPool *ttp = NULL;
        GError *err = NULL;

        if (!ttp)
        {
            if (!g_thread_supported ()) g_thread_init (NULL);
            ttp = g_thread_pool_new ((GFunc) mdv_thread, NULL, 4, TRUE, &err);
            g_assert(!err);
        }
        g_thread_pool_push (ttp, g_string_new (ceos_file), &err);
        g_assert(!err);
#endif
    }
    else
    {
        message_box("Failed to open external metadata viewer!");
    }

    g_free (ceos_file);
}

void
metadata_hide()
{
    GtkWidget *metadata_dialog =
        glade_xml_get_widget(glade_xml, "metadata_dialog");

    gtk_widget_hide(metadata_dialog);
}


SIGNAL_CALLBACK void
on_metadata_dialog_ok_button_clicked(GtkWidget *widget)
{
    metadata_hide();
}

SIGNAL_CALLBACK gboolean
on_metadata_dialog_delete_event(GtkWidget *widget)
{
    metadata_hide();
    return TRUE;
}

SIGNAL_CALLBACK gboolean
on_metadata_dialog_destroy_event(GtkWidget *widget)
{
    metadata_hide();
    return TRUE;
}

SIGNAL_CALLBACK gboolean
on_metadata_dialog_destroy(GtkWidget *widget)
{
    metadata_hide();
    return TRUE;
}

SIGNAL_CALLBACK gboolean
on_metadata_dialog_key_press_event(GtkWidget * widget, 
                                   GdkEventKey * event,
                                   GtkWidget * win)
{
    if (event->keyval == GDK_Return)
    {
        metadata_hide();
        return TRUE;
    }

    return FALSE;
}
