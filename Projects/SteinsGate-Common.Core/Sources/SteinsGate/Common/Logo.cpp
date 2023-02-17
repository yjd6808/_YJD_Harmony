/*
 * 작성자: 윤정도
 * 생성일: 2/17/2023 4:41:49 PM
 * =====================
 *
 */


#include "Core.h"
#include "Logo.h"

USING_NS_JC;
USING_NS_JS;

SGString Logo_v{2048};
const int LogoWidth_v = 600;
const int LogoHeight_v = 800;
/* ================================================================================================================
 *                                     로고 스트링 컨케터네이션
 * ================================================================================================================ */

#define COLOR_BLACK			CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_BLACK		, CSI_GRAPHIC_RENDITION_END
#define COLOR_LIGHT_RED		CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_RED			, CSI_GRAPHIC_RENDITION_END
#define COLOR_WHITE			CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_WHITE		, CSI_GRAPHIC_RENDITION_END
#define COLOR_GRAY			CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_GRAY			, CSI_GRAPHIC_RENDITION_END
#define COLOR_LIGHT_GREEN	CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_LIGHT_GREEN	, CSI_GRAPHIC_RENDITION_END
#define COLOR_LIGHT_YELLOW	CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_LIGHT_YELLOW	, CSI_GRAPHIC_RENDITION_END
#define COLOR_CYAN			CSI, VT_RESET, VT_BACK_COLOR_BLACK, CSI_AND, VT_FORE_COLOR_CYAN			, CSI_GRAPHIC_RENDITION_END

void InitializeClientLogo() {

}

void InitializeServerAuthLogo() {
	Logo_v.SetLength(0);
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"                     ",  COLOR_WHITE,		 "                                          \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"                     ",  COLOR_WHITE,		 "                                          \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"             █████╗  ",  COLOR_WHITE,		 "██╗   ██╗████████╗██╗  ██╗                \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"             ██╔══██╗",  COLOR_WHITE,		 "██║   ██║╚══██╔══╝██║  ██║                \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"             ███████║",  COLOR_WHITE,		 "██║   ██║   ██║   ███████║                \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"             ██╔══██║",  COLOR_WHITE,		 "██║   ██║   ██║   ██╔══██║                \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"             ██║  ██║",  COLOR_WHITE,		 "╚██████╔╝   ██║   ██║  ██║                \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_RED   ,"             ╚═╝  ╚═╝",  COLOR_WHITE,		 " ╚═════╝    ╚═╝   ╚═╝  ╚═╝                \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_GREEN ,"             --───▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬───--              \n");
	Logo_v += MoveConcat<SGString>(COLOR_LIGHT_GREEN ,"                                                               \n");
	Logo_v += MoveConcat<SGString>(CSI_GRAPHIC_RENDITION(0));
}

void InitializeServerLobbyLogo() {

}


void InitializeServerGameLogo() {
	
}

void InitializeServerCenterLogo() {

}



/* ================================================================================================================
 *                                     로고 기본 테스트
 * ================================================================================================================

 
printf("                                                               \n");
printf("                                                               \n");
printf("            ██████╗  █████╗ ███╗   ███╗███████╗                \n");
printf("           ██╔════╝ ██╔══██╗████╗ ████║██╔════╝                \n");
printf("           ██║  ███╗███████║██╔████╔██║█████╗                  \n");
printf("           ██║   ██║██╔══██║██║╚██╔╝██║██╔══╝                  \n");
printf("           ╚██████╔╝██║  ██║██║ ╚═╝ ██║███████╗                \n");
printf("            ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝                \n");
printf("                                                               \n");


printf("                                                               \n");
printf("                                                               \n");
printf("       ██████╗███████╗███╗  ██╗████████╗███████╗██████╗        \n");
printf("     ██╔════╝██╔════╝████╗  ██║╚══██╔══╝██╔════╝██╔══██╗       \n");
printf("     ██║     █████╗  ██╔██╗ ██║   ██║   █████╗  ██████╔╝       \n");
printf("     ██║     ██╔══╝  ██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗       \n");
printf("     ╚██████╗███████╗██║ ╚████║   ██║   ███████╗██║  ██║       \n");
printf("      ╚═════╝╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝       \n");
printf("                                                               \n");

printf("                                                               \n");
printf("                                                               \n");
printf("         ██╗      ██████╗ ██████╗ ██████╗ ██╗   ██╗            \n");
printf("         ██║     ██╔═══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝            \n");
printf("         ██║     ██║   ██║██████╔╝██████╔╝ ╚████╔╝             \n");
printf("         ██║     ██║   ██║██╔══██╗██╔══██╗  ╚██╔╝              \n");
printf("         ███████╗╚██████╔╝██████╔╝██████╔╝   ██║               \n");
printf("         ╚══════╝ ╚═════╝ ╚═════╝ ╚═════╝    ╚═╝               \n");
printf("                                                               \n");
*/


/* ================================================================================================================
 *                                     로고 컬러라이징 테스트
 * ================================================================================================================

 

#define COLOR_BLACK			CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_BLACK		CSI_GRAPHIC_RENDITION_END
#define COLOR_LIGHT_RED		CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_RED			CSI_GRAPHIC_RENDITION_END
#define COLOR_WHITE			CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_WHITE		CSI_GRAPHIC_RENDITION_END
#define COLOR_GRAY			CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_GRAY			CSI_GRAPHIC_RENDITION_END
#define COLOR_LIGHT_GREEN	CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_LIGHT_GREEN	CSI_GRAPHIC_RENDITION_END
#define COLOR_LIGHT_YELLOW	CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_LIGHT_YELLOW	CSI_GRAPHIC_RENDITION_END
#define COLOR_CYAN			CSI VT_RESET VT_BACK_COLOR_BLACK CSI_AND VT_FORE_COLOR_CYAN			CSI_GRAPHIC_RENDITION_END

printf(COLOR_LIGHT_RED   "                     "  COLOR_WHITE		 "                                          \n");
printf(COLOR_LIGHT_RED   "                     "  COLOR_WHITE		 "                                          \n");
printf(COLOR_LIGHT_RED   "             █████╗  "  COLOR_WHITE		 "██╗   ██╗████████╗██╗  ██╗                \n");
printf(COLOR_LIGHT_RED   "             ██╔══██╗"  COLOR_WHITE		 "██║   ██║╚══██╔══╝██║  ██║                \n");
printf(COLOR_LIGHT_RED   "             ███████║"  COLOR_WHITE		 "██║   ██║   ██║   ███████║                \n");
printf(COLOR_LIGHT_RED   "             ██╔══██║"  COLOR_WHITE		 "██║   ██║   ██║   ██╔══██║                \n");
printf(COLOR_LIGHT_RED   "             ██║  ██║"  COLOR_WHITE		 "╚██████╔╝   ██║   ██║  ██║                \n");
printf(COLOR_LIGHT_RED   "             ╚═╝  ╚═╝"  COLOR_WHITE		 " ╚═════╝    ╚═╝   ╚═╝  ╚═╝                \n");
printf(COLOR_LIGHT_GREEN "             --───▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬───--              \n");
printf(COLOR_LIGHT_GREEN "                                                               \n");
printf(CSI_GRAPHIC_RENDITION(0));

printf(COLOR_LIGHT_YELLOW "                   " COLOR_WHITE "                                            \n");
printf(COLOR_LIGHT_YELLOW "                   " COLOR_WHITE "                                            \n");
printf(COLOR_LIGHT_YELLOW "            ██████╗" COLOR_WHITE "  █████╗ ███╗   ███╗███████╗                \n");
printf(COLOR_LIGHT_YELLOW "           ██╔════╝" COLOR_WHITE " ██╔══██╗████╗ ████║██╔════╝                \n");
printf(COLOR_LIGHT_YELLOW "           ██║  ███" COLOR_WHITE "╗███████║██╔████╔██║█████╗                  \n");
printf(COLOR_LIGHT_YELLOW "           ██║   ██" COLOR_WHITE "║██╔══██║██║╚██╔╝██║██╔══╝                  \n");
printf(COLOR_LIGHT_YELLOW "           ╚██████╔" COLOR_WHITE "╝██║  ██║██║ ╚═╝ ██║███████╗                \n");
printf(COLOR_LIGHT_YELLOW "            ╚═════╝" COLOR_WHITE " ╚═╝  ╚═╝╚═╝     ╚═╝╚══════╝                \n");
printf(COLOR_LIGHT_GREEN "             --───▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬───--              \n");
printf(COLOR_LIGHT_GREEN "                                                               \n");
printf(CSI_GRAPHIC_RENDITION(0));

printf(COLOR_LIGHT_GREEN "                 " COLOR_WHITE "                                              \n");
printf(COLOR_LIGHT_GREEN "                 " COLOR_WHITE "                                              \n");
printf(COLOR_LIGHT_GREEN "         ██╗     " COLOR_WHITE "  ██████╗ ██████╗ ██████╗ ██╗   ██╗           \n");
printf(COLOR_LIGHT_GREEN "         ██║     " COLOR_WHITE " ██╔═══██╗██╔══██╗██╔══██╗╚██╗ ██╔╝           \n");
printf(COLOR_LIGHT_GREEN "         ██║     " COLOR_WHITE " ██║   ██║██████╔╝██████╔╝ ╚████╔╝            \n");
printf(COLOR_LIGHT_GREEN "         ██║     " COLOR_WHITE " ██║   ██║██╔══██╗██╔══██╗  ╚██╔╝             \n");
printf(COLOR_LIGHT_GREEN "         ███████╗" COLOR_WHITE " ╚██████╔╝██████╔╝██████╔╝   ██║              \n");
printf(COLOR_LIGHT_GREEN "         ╚══════╝" COLOR_WHITE "  ╚═════╝ ╚═════╝ ╚═════╝    ╚═╝              \n");
printf(COLOR_LIGHT_GREEN "             --───▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬───--              \n");
printf(COLOR_LIGHT_GREEN "                                                               \n");
printf(CSI_GRAPHIC_RENDITION(0));

printf(COLOR_CYAN "             " COLOR_WHITE "                                                  \n");
printf(COLOR_CYAN "             " COLOR_WHITE "                                                  \n");
printf(COLOR_CYAN "       ██████" COLOR_WHITE "╗███████╗███╗  ██╗████████╗███████╗██████╗        \n");
printf(COLOR_CYAN "     ██╔════╝" COLOR_WHITE "██╔════╝████╗  ██║╚══██╔══╝██╔════╝██╔══██╗       \n");
printf(COLOR_CYAN "     ██║     " COLOR_WHITE "█████╗  ██╔██╗ ██║   ██║   █████╗  ██████╔╝       \n");
printf(COLOR_CYAN "     ██║     " COLOR_WHITE "██╔══╝  ██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗       \n");
printf(COLOR_CYAN "     ╚██████╗" COLOR_WHITE "███████╗██║ ╚████║   ██║   ███████╗██║  ██║       \n");
printf(COLOR_CYAN "      ╚═════╝" COLOR_WHITE "╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝       \n");
printf(COLOR_LIGHT_GREEN "             --───▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬▬───--              \n");
printf(COLOR_LIGHT_GREEN "                                                               \n");
printf(CSI_GRAPHIC_RENDITION(0));

*/