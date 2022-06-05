#include <stdlib.h>
#include <stdio.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <xcb/xcb.h>
#include <xcb/shm.h>
#include <xcb/xcb_image.h>

#include <unistd.h>

#include <cairo/cairo-xcb.h>

#define WID 512
#define HEI 512

int main()
{
    xcb_connection_t *connection;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_gcontext_t gcontext;
    xcb_generic_event_t *event;

    uint32_t value_mask;
    uint32_t value_list[2];

    // connect to the X server and get screen

    connection = xcb_connect(NULL, NULL);
    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;

    // create a window

    value_mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    value_list[0] = screen->black_pixel;
    value_list[1] = XCB_EVENT_MASK_EXPOSURE;

    window = xcb_generate_id(connection);

    xcb_create_window(
        connection,
        screen->root_depth,
        window,
        screen->root,
        0, 0,
        WID, HEI,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        value_mask, value_list);

    // create a graphic context

    value_mask = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
    value_list[0] = screen->black_pixel;
    value_list[1] = 0;

    gcontext = xcb_generate_id(connection);
    xcb_create_gc(connection, gcontext, window, value_mask, value_list);

    // map the window onto the screen

    xcb_map_window(connection, window);
    xcb_flush(connection);

    // Shm test
    xcb_shm_query_version_reply_t *reply;
    xcb_shm_segment_info_t info;

    reply = xcb_shm_query_version_reply(
        connection,
        xcb_shm_query_version(connection),
        NULL);

    if (!reply || !reply->shared_pixmaps)
    {
        printf("Shm error...\n");
        exit(0);
    }

    info.shmid = shmget(IPC_PRIVATE, WID * HEI * 4, IPC_CREAT | 0600);
    info.shmaddr = static_cast<uint8_t *>(shmat(info.shmid, 0, 0));

    info.shmseg = xcb_generate_id(connection);
    xcb_shm_attach(connection, info.shmseg, info.shmid, 0);
    shmctl(info.shmid, IPC_RMID, 0);

    uint32_t *data = reinterpret_cast<uint32_t *>(info.shmaddr);

    xcb_pixmap_t pix = xcb_generate_id(connection);
    xcb_shm_create_pixmap(
        connection,
        pix,
        window,
        WID, HEI,
        screen->root_depth,
        info.shmseg,
        0);

    // Cairo stuff
    auto stride = cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, WID);
    auto s = cairo_image_surface_create_for_data(
        info.shmaddr, CAIRO_FORMAT_ARGB32, WID, HEI, stride);
    auto cr = cairo_create(s);

    int i = 0;
    while (1)
    {
        usleep(10000);

        // data[i] = 0xFFFFFF;
        // Cairo stuff
        cairo_set_source_rgb(cr, 0.5, 0.5, 1);
        cairo_rectangle(cr, i % 20, i % 20, 100, 100);
        cairo_fill(cr);

        i++;

        xcb_copy_area(
            connection,
            pix,
            window,
            gcontext,
            0, 0, 0, 0,
            WID, HEI);

        xcb_flush(connection);
    }

    xcb_shm_detach(connection, info.shmseg);
    shmdt(info.shmaddr);

    xcb_free_pixmap(connection, pix);

    xcb_destroy_window(connection, window);
    xcb_disconnect(connection);

    return 0;
}
