#include <boost/asio.hpp>

#include <controller.h>

/*
  Status: disconnected / connected to Hungary / connecting to Hungary
  --
  Disconnect from Hungary (only if it's connected)
  --
  Connect to ... >
    Smart location (Sweden - 2) (smart)
    Pick for Me (xv)
    --
    Africa >
    Asia >
    Europe >
      Hungary
      Netherlands >
        Netherlands - Amsterdam
        Netherlands - Rotterdam
    North America >
    South America >
    Note: if there are  recommended servers, list them first then add separator and list the rest
 --
 Refresh server list
 --
 Quit
 */



// void on_checkbox_toggled(GtkCheckMenuItem *check_menu_item, gpointer user_data)
// {
//     gboolean active = gtk_check_menu_item_get_active(check_menu_item);
//     g_print("Checkbox toggled: %s\n", active ? "checked" : "unchecked");
// }

// void on_menu_item_selected(GtkMenuItem *menu_item, gpointer user_data)
// {
//     // GtkRadioMenuItem * item = menu_item;
//     bool active = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (menu_item));
//     const gchar *label = gtk_menu_item_get_label(menu_item);
//     g_print("Selected item: %s %d\n", label, active);
// }


int main(int argc, char **argv)
{
    
    boost::asio::io_context ctx;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> workGuard{boost::asio::make_work_guard(ctx)};

    unsigned int const numOfThreads{2};
    boost::asio::thread_pool pool(numOfThreads);

    for (auto i=0u; i<numOfThreads; ++i)
    {
        boost::asio::post(pool,
                          [&ctx] ()
                          {
                              ctx.run();
                          });
    }
    
    Controller controller(ctx);


    pool.join();
    
//    ctx.stop(); // finish asap, queued tasks won't be processed, deadline_timer completion handler is not even called


    

//     app_indicator_set_attention_icon_full(indicator, "indicator-messages-new", "System Messages Icon Highlighted");
//     app_indicator_set_label (indicator, "1%", "100%");
//     app_indicator_set_title (indicator, "Test Inidcator");
        

//     // Create menu

//     GtkWidget *disabled_label = gtk_menu_item_new_with_label("Disabled");
//     gtk_widget_set_sensitive(disabled_label, FALSE); // Make it non-selectable
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), disabled_label);

//     GtkWidget *check_item = gtk_check_menu_item_new_with_label("Check Item");
//     g_signal_connect(check_item, "toggled", G_CALLBACK(on_checkbox_toggled), NULL);
    
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), check_item);


//     // Create radio menu items
//     GtkWidget *item1 = gtk_radio_menu_item_new_with_label(NULL, "Item 1");
//     GtkWidget *item2 = gtk_radio_menu_item_new_with_label_from_widget(GTK_RADIO_MENU_ITEM(item1), "Item 2");

//     gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (item2), TRUE);

//     // Connect callback function to handle item selection
//     g_signal_connect(item1, "activate", G_CALLBACK(on_menu_item_selected), NULL);
//     g_signal_connect(item2, "activate", G_CALLBACK(on_menu_item_selected), NULL);

//     // Append menu items to the menu
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), item1);
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), item2);

    
//     GtkWidget *header_separator = gtk_separator_menu_item_new();
//     gtk_menu_item_set_label(GTK_MENU_ITEM(header_separator), "Header");
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), header_separator);
 
//     GtkWidget * subMenuItem = gtk_menu_item_new_with_label("SubMenu");
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), subMenuItem);

//     GtkWidget * subMenu = gtk_menu_new();
//     GtkWidget * subMenuItem1 = gtk_menu_item_new_with_label("1");
//     gtk_menu_shell_append(GTK_MENU_SHELL(subMenu), subMenuItem1);
//     GtkWidget * subMenuItem2 = gtk_menu_item_new_with_label("2");
//     gtk_menu_shell_append(GTK_MENU_SHELL(subMenu), subMenuItem2);
    
//     gtk_menu_item_set_submenu((GtkMenuItem*)subMenuItem, subMenu);

//     GtkWidget *menu_item_separator = gtk_separator_menu_item_new();
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item_separator);
    
//     GtkWidget *menu_item_quit = gtk_menu_item_new_with_label("Quit");
//     g_signal_connect(menu_item_quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
//     gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item_quit);

//     gtk_widget_show_all(menu);

//     // Attach menu to indicator
//     app_indicator_set_menu(indicator, GTK_MENU(menu));

//     // Start GTK main loop
//     gtk_main();

    return 0;
}
