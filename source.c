// gtklock-powerbar-module
// Copyright (c) 2022 Jovan Lanik

// Power module

#include "gtklock-module.h"

#define MODULE_DATA(x) (x->module_data[self_id])
#define POWERBAR(x) ((struct powerbar *)MODULE_DATA(x))

extern void config_load(const char *path, const char *group, GOptionEntry entries[]);

struct powerbar {
	GtkWidget *revealer;
	GtkWidget *box;
	GtkWidget *reboot_button;
	GtkWidget *poweroff_button;
};

static int self_id;

static gboolean show_labels = FALSE;
static gchar *reboot_command = "systemctl reboot";
static gchar *poweroff_command = "systemctl -i poweroff";

static GOptionEntry powerbar_entries[] = {
	{ "show-labels", 0, 0, G_OPTION_ARG_NONE, &show_labels, NULL, NULL },
	{ "reboot-command", 0, 0, G_OPTION_ARG_STRING, &reboot_command, NULL, NULL },
	{ "poweroff-command", 0, 0, G_OPTION_ARG_STRING, &poweroff_command, NULL, NULL },
	{ NULL },
};

static void button_clicked(GtkButton *self, gpointer user_data) {
	g_spawn_command_line_async(user_data, NULL);
}

static void setup_powerbar(struct Window *ctx) {
	if(MODULE_DATA(ctx) != NULL) {
		gtk_widget_destroy(POWERBAR(ctx)->revealer);
		g_free(MODULE_DATA(ctx));
		MODULE_DATA(ctx) = NULL;
	}
	MODULE_DATA(ctx) = g_malloc(sizeof(struct powerbar));

	POWERBAR(ctx)->revealer = gtk_revealer_new();
	g_object_set(POWERBAR(ctx)->revealer, "margin", 5, NULL);
	gtk_widget_set_halign(POWERBAR(ctx)->revealer, GTK_ALIGN_CENTER);
	gtk_widget_set_valign(POWERBAR(ctx)->revealer, GTK_ALIGN_END);
	gtk_widget_set_name(POWERBAR(ctx)->revealer, "powerbar-revealer");
	gtk_revealer_set_reveal_child(GTK_REVEALER(POWERBAR(ctx)->revealer), TRUE);
	gtk_revealer_set_transition_type(GTK_REVEALER(POWERBAR(ctx)->revealer), GTK_REVEALER_TRANSITION_TYPE_NONE);
	gtk_overlay_add_overlay(GTK_OVERLAY(ctx->overlay), POWERBAR(ctx)->revealer);

	POWERBAR(ctx)->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_halign(POWERBAR(ctx)->box, GTK_ALIGN_CENTER);
	gtk_widget_set_name(POWERBAR(ctx)->box, "powerbar-box");
	gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->revealer), POWERBAR(ctx)->box);

	POWERBAR(ctx)->reboot_button = gtk_button_new_from_icon_name("view-refresh", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(POWERBAR(ctx)->reboot_button, "reboot-button");
	//gtk_widget_set_tooltip_text(POWERBAR(ctx)->reboot_button, "Reboot");
	gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->reboot_button), TRUE);
	if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->reboot_button), "Reboot");
	g_signal_connect(POWERBAR(ctx)->reboot_button, "clicked", G_CALLBACK(button_clicked), reboot_command);
	gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->reboot_button);

	POWERBAR(ctx)->poweroff_button = gtk_button_new_from_icon_name("process-stop", GTK_ICON_SIZE_BUTTON);
	gtk_widget_set_name(POWERBAR(ctx)->poweroff_button, "poweroff-button");
	//gtk_widget_set_tooltip_text(POWERBAR(ctx)->poweroff_button, "Poweroff");
	gtk_button_set_always_show_image(GTK_BUTTON(POWERBAR(ctx)->poweroff_button), TRUE);
	if(show_labels) gtk_button_set_label(GTK_BUTTON(POWERBAR(ctx)->poweroff_button), "Poweroff");
	g_signal_connect(POWERBAR(ctx)->poweroff_button, "clicked", G_CALLBACK(button_clicked), poweroff_command);
	gtk_container_add(GTK_CONTAINER(POWERBAR(ctx)->box), POWERBAR(ctx)->poweroff_button);

	gtk_widget_show_all(POWERBAR(ctx)->revealer);
}

void g_module_unload(GModule *m) {
}

void on_activation(struct GtkLock *gtklock, int id) {
	self_id = id;
	config_load(gtklock->config_path, "powerbar", powerbar_entries);
}

void on_focus_change(struct GtkLock *gtklock, struct Window *win, struct Window *old) {
	setup_powerbar(win);
	if(gtklock->hidden)
		gtk_revealer_set_reveal_child(GTK_REVEALER(POWERBAR(win)->revealer), FALSE);
	if(old != NULL && win != old)
		gtk_revealer_set_reveal_child(GTK_REVEALER(POWERBAR(old)->revealer), FALSE);
}

void on_window_empty(struct GtkLock *gtklock, struct Window *ctx) {
	if(MODULE_DATA(ctx) != NULL) {
		g_free(MODULE_DATA(ctx));
		MODULE_DATA(ctx) = NULL;
	}
}

void on_idle_hide(struct GtkLock *gtklock) {
	if(gtklock->focused_window) {
		GtkRevealer *revealer = GTK_REVEALER(POWERBAR(gtklock->focused_window)->revealer);	
		gtk_revealer_set_reveal_child(revealer, FALSE);
	}
}

void on_idle_show(struct GtkLock *gtklock) {
	if(gtklock->focused_window) {
		GtkRevealer *revealer = GTK_REVEALER(POWERBAR(gtklock->focused_window)->revealer);	
		gtk_revealer_set_reveal_child(revealer, TRUE);
	}
}

