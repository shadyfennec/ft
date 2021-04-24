#include "ansi.h"
#include <stdlib.h>
#include <string.h>
#include "util.h"

void single_char_seq(char char_seq, struct EscapeSequence* seq) {
    seq->seq = SingleCharacterSequence;
    seq->single_character_seq = char_seq;
}

void literal_string_seq(char* ptr, struct EscapeSequence* seq) {
    seq->seq = LiteralString;
    seq->literal_str = ptr;
}

int is_num(char c) {
    return c >= 48 && c <= 57;
}

int read_number(char* start, int* number) {
    int len = 0;
    *number = 0;
    char c = start[len];
    while(is_num(c)) {
	*number *= 10;
	*number += (int)(c - 48);
	c = start[++len];
    }

    return len;
}

int read_st_terminated_string(char* start) {
    int idx = 0;
    while ((unsigned char)start[idx] != ANSI_BELL && (unsigned char)start[idx++] != ANSI_ST) {};
    return idx;
}

void copy_str(struct EscapeSequence* seq) {
    memcpy(seq->literal_str_buffer, seq->literal_str, seq->literal_str_len);
    seq->literal_str_buffer[seq->literal_str_len] = 0;
}

int read_osc(char* start, struct EscapeSequence* seq) {
    unsigned char command = start[0];

    switch (command) {
    case ANSI_OSC_WINDOW_TITLE:
	seq->seq = OSCSetWindowTitle;
	seq->literal_str = &start[2];
	seq->literal_str_len = read_st_terminated_string(&start[2]);
	copy_str(seq);
	return seq->literal_str_len + 2;
    default:
	DEBUG_LOG("Unknown OSC command: %c\n", command);
	return 1;
    }
}

int escape_seq(char* start, struct EscapeSequence* seq) {
    switch((unsigned char)start[1]) {
    case ANSI_OSC:
	return read_osc(&start[2], seq) + 2;
    case ANSI_CSI:
	break;
    default:
	DEBUG_LOG("Unknown start of escape sequence %c\n", start[1]);
	break;
    }

    int len = 2;
    unsigned char c;
    int cont = 1;
    int arg_count = 0;

    while (cont) {
	cont = 0;
	c = start[len];
	
	switch (c) {
	case 'f':
	case 'H':
	    {
		if (arg_count > 0) {
		    seq->seq = MoveCursorToLineColumn;
		} else {
		    seq->seq = MoveCursorToHome;
		}
		len++;
	    }
	    break;
	case 'A':
	    seq->seq = MoveCursorUp;
	    len++;
	    break;
	case 'B':
	    seq->seq = MoveCursorDown;
	    len++;
	    break;
	case 'C':
	    seq->seq = MoveCursorRight;
	    len++;
	    break;
	case 'D':
	    seq->seq = MoveCursorLeft;
	    len++;
	    break;
	case 'E':
	    seq->seq = MoveCursorUpBeginning;
	    len++;
	    break;
	case 'F':
	    seq->seq = MoveCursorDownBeginning;
	    len++;
	    break;
	case 'G':
	    seq->seq = MoveCursorToColumn;
	    len++;
	    break;
	case 'R':
	    seq->seq = ReportCursorPos;
	    len++;
	    break;
	case 's':
	    seq->seq = SaveCursorPos;
	    len++;
	    break;
	case 'u':
	    seq->seq = RestoreCursorPos;
	    len++;
	    break;
	case 'J':
	    {
		if (arg_count > 0) {
		    switch (seq->args[0]) {
		    case 0:
			seq->seq = ClearScreenFromCursorToEnd;
			break;
		    case 1:
			seq->seq = ClearScreenFromCursorToBeginning;
			break;
		    case 2:
			seq->seq = ClearEntireScreen;
			break;
		    default:
			DEBUG_LOG("Unknown clear screen argument %d\n", seq->args[0]);
			seq->seq = ClearScreen;
			break;
		    }
		} else {
		    seq->seq = ClearScreen;
		}
		len++;
	    }
	    break;
	case 'K':
	    {
		if (arg_count > 0) {
		    switch (seq->args[0]) {
		    case 0:
			seq->seq = ClearLineFromCursorToEnd;
			break;
		    case 1:
			seq->seq = ClearLineFromCursorToBeginning;
			break;
		    case 2:
			seq->seq = ClearEntireLine;
			break;
		    default:
			DEBUG_LOG("Unknown clear line argument %d\n", seq->args[0]);
			seq->seq = ClearLine;
			break;
		    }
		} else {
		    seq->seq = ClearLine;
		}
		len++;
	    }
	    break;
	case 'm':
	    {
		switch (seq->args[0]) {
		case 0:
		    seq->seq = ModeReset;
		    break;
		case 1:
		    seq->seq = ModeBold;
		    break;
		case 2:
		    seq->seq = ModeFaint;
		    break;
		case 3:
		    seq->seq = ModeItalic;
		    break;
		case 4:
		    seq->seq = ModeUnderline;
		    break;
		case 5:
		    seq->seq = ModeBlinking;
		    break;
		case 7:
		    seq->seq = ModeReverse;
		    break;
		case 8:
		    seq->seq = ModeInvisible;
		    break;
		case 9:
		    seq->seq = ModeStrikethrough;
		    break;
		default:
		    DEBUG_LOG("Unknown mode %d\n", seq->args[0]);
		    seq->seq = ModeReset;
		    break;
		}
	    }
	    len++;
	    break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	    len += read_number(&start[len], &seq->args[arg_count++]);
	    cont = 1;
	    break;
	case ';':
	    len++;
	    cont = 1;
	    break;
	case '?':
	    seq->seq = PrivateSequence;
	    len++;
	    cont = 1;
	    break;
	case 'h':
	    {
		if (seq->args[0] == 2004) {
		    seq->seq = TurnOnBracketedMode;
		} else {
		    DEBUG_LOG("Unknown private sequence ESC[?%dh", seq->args[0]);
		    seq->seq = Unknown;
		}
		len++;
	    }
	    break;
	case 'l':
	    {
		if (seq->args[0] == 2004) {
		    seq->seq = TurnOffBracketedMode;
		} else {
		    DEBUG_LOG("Unknown private sequence ESC[?%dl", seq->args[0]);
		    seq->seq = Unknown;
		}
		len++;
	    }
	    break;
	default:
	    DEBUG_LOG("Unknown control sequence character: %c\n", c);
	    seq->seq = Unknown;
	    len++;
	    break;
	}
    }

    return len;
}

int is_sequence_starter(unsigned char c) {
    return c == ANSI_BELL ||
	c == ANSI_BS ||
	c == ANSI_TAB ||
	c == ANSI_LF ||
	c == ANSI_FF ||
	c == ANSI_CR ||
	c == ANSI_DEL ||
	c == ANSI_ESC;
}

int read_ansi_sequence(char *str, unsigned int len, struct EscapeSequence *seq) {
    int idx = 0;
    char c;

    seq->literal_str = NULL;
    int is_str = 0;
    
    while(1) {
	c = str[idx];
	
	switch (c) {
	case ANSI_BELL:
	case ANSI_BS:
	case ANSI_TAB:
	case ANSI_LF:
	case ANSI_FF:
	case ANSI_CR:
	case ANSI_DEL:
	    single_char_seq(c, seq);
	    return 1;

        case ANSI_ESC:
	    seq->seq = Unknown;
	    return escape_seq(str, seq);
	    
	default:
	    {
		if (!is_str) {
		    is_str = 1;
		    seq->seq = LiteralString;
		    seq->literal_str = &str[idx];
		}
		
		int len = get_utf_8_len(c);
		int str_len = idx + len;

		if (str_len >= len || is_sequence_starter(str[str_len])) {
		    seq->literal_str_len = str_len;
		    copy_str(seq);
		    return str_len; 
	        }
	        
		idx += len;
	    }
	    break;
	}
    }
}
/*
int interpret_escape_sequences(char *str, unsigned int len, struct EscapeSequence *buf) {
    if (len == 0) {
	return 0;
    }

    int buf_pos = 0;
    int str_pos = 0;
    int last_str_seq = -1;
    
    while(str_pos < len) {
	char c = str[str_pos];

	switch (c) {
	case ANSI_BELL:
	case ANSI_BS:
	case ANSI_TAB:
	case ANSI_LF:
	case ANSI_FF:
	case ANSI_CR:
	case ANSI_DEL:
	    if (last_str_seq >= 0) {
		buf[buf_pos - 1].literal_str_len = str_pos - last_str_seq;
		last_str_seq = -1;
	    }
	    single_char_seq(c, &buf[buf_pos++]);
	    str_pos++;
	    break;

        case ANSI_ESC:
	    if (last_str_seq >= 0) {
		buf[buf_pos - 1].literal_str_len = str_pos - last_str_seq;
		last_str_seq = -1;
	    }
	    buf[buf_pos].seq = Unknown;
	    str_pos += escape_seq(&str[str_pos], &buf[buf_pos++]) + 1;
	    break;
	    
	default:
	    if (last_str_seq < 0) {
		last_str_seq = str_pos;
		literal_string_seq(&str[last_str_seq], &buf[buf_pos++]);
	    }
	    str_pos+= get_utf_8_len(c);
	    break;
	}
    }
    
    return buf_pos;
}
*/
