#ifndef HYDROOS_ELEMENTS_H
#define HYDROOS_ELEMENTS_H

#include <stdint.h>

typedef struct {
    int atomic_number;
    char symbol[4];
    char docker_image[32];
    uint32_t container_id;
    int is_active;
} HydroDockerContainer;

void show_carbon_app(void);
void show_boron_app(void);
void show_titanium_app(void);
void show_phosphorus_meeting_app(void);
void show_fluorine_form_app(void);
void show_titanium_clock_app(void);
void show_boron_calc_app(void);
void show_calcium_store_app(void);
void show_silicon_book_app(void);
void show_helium_video_app(void);
void show_lithium_chat_app(void);
void show_potassium_classroom(void);
void show_magnesium_family(void);
void show_iodine_map_app(void);
void show_silver_translate_app(void);
void show_tungsten_journal_app(void);
void show_lead_fonts_app(void);
void show_silicon_settings_app(void);
void show_moscovium_task_app(void);
void show_iron_explorer_app(void);
void show_iron_files_app(void);
void show_gold_wallet_app(void);
void show_oxygen_paint_app(void);

void show_rutherfordium_scheduler(void);
void show_polonium_log_app(void);
void show_technetium_debugger(void);
void show_gallium_io_app(void);

#endif /* HYDROOS_ELEMENTS_H */
