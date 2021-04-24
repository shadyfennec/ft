#ifndef __ANSI_H__
#define __ANSI_H__

/* Single character control sequences characters */
#define ANSI_BELL 0x7
#define ANSI_BS 0x8
#define ANSI_TAB 0x9
#define ANSI_LF 0xA
#define ANSI_FF 0xC
#define ANSI_CR 0xD
#define ANSI_DEL 0x7F

/* FE escape control sequences characters */
#define ANSI_ESC 0x1B
#define ANSI_SS2 0x8E
#define ANSI_SS3 0x8F
#define ANSI_DCS 0x90
#define ANSI_CSI '['
#define ANSI_ST '\\'
#define ANSI_OSC ']'
#define ANSI_SOS 0x98
#define ANSI_PM 0x9E
#define ANSI_AOC 0x9F

/* CSI escape control sequences */
enum ANSI_CSI_SEQUENCE {
  Unknown,
  LiteralString,                    /* No sequence, plain text */
  SingleCharacterSequence,          /* see above */
  MoveCursorToHome,                 /* ESC[H */
  MoveCursorToLineColumn,           /* ESC[{l};{c}H or ESC[{l};{c}f */
  MoveCursorUp,                     /* ESC[#A */
  MoveCursorDown,                   /* ESC[#B */
  MoveCursorRight,                  /* ESC[#C */
  MoveCursorLeft,                   /* ESC[#D */
  MoveCursorUpBeginning,            /* ESC[#E */
  MoveCursorDownBeginning,          /* ESC[#F */
  MoveCursorToColumn,               /* ESC[#G */
  ReportCursorPos,                  /* ESC[#;#R */
  SaveCursorPos,                    /* ESC[s */
  RestoreCursorPos,                 /* ESC[u */
  ClearScreen,                      /* ESC[J */
  ClearScreenFromCursorToEnd,       /* ESC[0J */
  ClearScreenFromCursorToBeginning, /* ESC[1J */
  ClearEntireScreen,                /* ESC[2J */
  ClearLine,                        /* ESC[K */
  ClearLineFromCursorToEnd,         /* ESC[0K */
  ClearLineFromCursorToBeginning,   /* ESC[1K */
  ClearEntireLine,                  /* ESC[2K */
  ModeReset,                        /* ESC[0m */
  ModeBold,                         /* ESC[1m */
  ModeFaint,                        /* ESC[2m */
  ModeItalic,                       /* ESC[3m */
  ModeUnderline,                    /* ESC[4m */
  ModeBlinking,                     /* ESC[5m */
  ModeReverse,                      /* ESC[7m */
  ModeInvisible,                    /* ESC[8m */
  ModeStrikethrough,                /* ESC[9m */
  PrivateSequence,                  /* ESC[?... */
  TurnOnBracketedMode,              /* ESC[?2004h */
  TurnOffBracketedMode,             /* ESC[?2004l */
  OSCSetWindowTitle,                /* ESC OSC ...\ */
};

/* OS Command codes */
#define ANSI_OSC_WINDOW_TITLE '0'
#define ANSI_OSC_HYPERLINK '8'

/* Color codes */
#define ANSI_COLOR_OFFSET 30
#define ANSI_FG_BLACK 30
#define ANSI_FG_RED 31
#define ANSI_FG_GREEN 32
#define ANSI_FG_YELLOW 33
#define ANSI_FG_BLUE 34
#define ANSI_FG_MAGENTA 35
#define ANSI_FG_CYAN 36
#define ANSI_FG_WHITE 37
#define ANSI_BG_BLACK 40
#define ANSI_BG_RED 41
#define ANSI_BG_GREEN 42
#define ANSI_BG_YELLOW 43
#define ANSI_BG_BLUE 44
#define ANSI_BG_MAGENTA 45
#define ANSI_BG_CYAN 46
#define ANSI_BG_WHITE 47
#define ANSI_COLOR_RESET 0

#define ANSI_SEQ_ARGS_SIZE 8

#define SEQUENCE_LITERAL_STR_LEN 512

struct EscapeSequence {
    enum ANSI_CSI_SEQUENCE seq;
    char single_character_seq;
    char* literal_str;
    char literal_str_buffer[SEQUENCE_LITERAL_STR_LEN];
    unsigned int literal_str_len;
    int args[ANSI_SEQ_ARGS_SIZE];
};

int read_ansi_sequence(char* str, unsigned int len, struct EscapeSequence* buf);

#endif
