/* Generated from DESIGN.md by tools/gen_theme.py. Do not edit: edit DESIGN.md. */
/* RGB565 codes; --verify checks contrast after quantisation, not just the source hex. */
#ifndef FELUCCA_THEME_H
#define FELUCCA_THEME_H

/* Normative colours, including the screen-safe variants. */
#define GUBHU_COLOR_PRIMARY 0x681Fu /* #6A00FF */
#define GUBHU_COLOR_SECONDARY 0xD0D1u /* #D71B8A */
#define GUBHU_COLOR_TERTIARY 0xFCC0u /* #FF9A00 */
#define GUBHU_COLOR_NEUTRAL 0xFF7Cu /* #F8EFE3 */
#define GUBHU_COLOR_INK 0x0802u /* #0B0316 */
#define GUBHU_COLOR_SURFACE 0x1044u /* #150B22 */
#define GUBHU_COLOR_SURFACE_RAISED 0x2085u /* #24132F */
#define GUBHU_COLOR_SURFACE_HOVER 0x30C8u /* #321844 */
#define GUBHU_COLOR_BORDER 0x51ABu /* #52365F */
#define GUBHU_COLOR_ON_SURFACE 0xFF7Cu /* #F8EFE3 */
#define GUBHU_COLOR_MUTED 0xC5B9u /* #C7B5CE */
#define GUBHU_COLOR_ON_PRIMARY 0xFFFFu /* #FFFFFF */
#define GUBHU_COLOR_ON_SECONDARY 0xFFFFu /* #FFFFFF */
#define GUBHU_COLOR_ON_TERTIARY 0x0802u /* #0B0316 */
#define GUBHU_COLOR_FUCHSIA 0xD0D1u /* #D71B8A */
#define GUBHU_COLOR_SUNSET 0xFAA9u /* #FF544E */
#define GUBHU_COLOR_AMBER 0xFCC0u /* #FF9A00 */
#define GUBHU_COLOR_MAIZE 0xFE86u /* #FFD234 */
#define GUBHU_COLOR_TURQUOISE 0x2638u /* #25C5C4 */
#define GUBHU_COLOR_SUCCESS 0x5694u /* #50D3A5 */
#define GUBHU_COLOR_WARNING 0xFE86u /* #FFD234 */
#define GUBHU_COLOR_ERROR 0xFB4Du /* #FF6969 */
#define GUBHU_COLOR_FOCUS 0xFE86u /* #FFD234 */
#define GUBHU_COLOR_LIGHT_SURFACE 0xFF7Cu /* #F8EFE3 */
#define GUBHU_COLOR_LIGHT_ON_SURFACE 0x0802u /* #0B0316 */
#define GUBHU_COLOR_LIGHT_MUTED 0x522Bu /* #54445D */
#define GUBHU_COLOR_PRIMARY_SCREEN 0x9ABEu /* #9C54F5 */
#define GUBHU_COLOR_SECONDARY_SCREEN 0xDA33u /* #DE459C */
#define GUBHU_COLOR_BORDER_SCREEN 0x72CFu /* #735B79 */

/* FM1 semantic roles (gfx.c's five steps, surfaces, accent and tracks). */
#define GUBHU_BG 0x0802u /* firmware.background: #0B0316 */
#define GUBHU_ACCENT 0xFE86u /* firmware.accent: #FFD234 */
#define GUBHU_PANEL 0x1044u /* firmware.panel: #150B22 */
#define GUBHU_EMPTY 0x2085u /* firmware.empty: #24132F */
#define GUBHU_STEP_OUTLINE 0x9ABEu /* firmware.empty-outline: #9C54F5 */
#define GUBHU_C_LINE 0x72CFu /* firmware.line: #735B79 */
#define GUBHU_C_DIM 0x9ABEu /* firmware.dim: #9C54F5 */
#define GUBHU_C_GRAY 0xC5B9u /* firmware.labels: #C7B5CE */
#define GUBHU_C_AMB 0xFE86u /* firmware.secondary: #FFD234 */
#define GUBHU_C_HI 0xFF7Cu /* firmware.values: #F8EFE3 */
#define GUBHU_TE_G1 0x2085u /* firmware.empty: #24132F */
#define GUBHU_TE_G2 0x1044u /* firmware.panel: #150B22 */
#define GUBHU_TE_G3 0xC5B9u /* firmware.labels: #C7B5CE */
#define GUBHU_TE_G4 0xFF7Cu /* firmware.values: #F8EFE3 */
#define GUBHU_TRACK_1 0x9ABEu /* firmware.track-1: #9C54F5 */
#define GUBHU_TRACK_2 0x2638u /* firmware.track-2: #25C5C4 */
#define GUBHU_TRACK_3 0xFE86u /* firmware.track-3: #FFD234 */
#define GUBHU_TRACK_4 0xDA33u /* firmware.track-4: #DE459C */

#endif
