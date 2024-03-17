#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <key.h>

struct termios orig_termios;


/*
TCSANOW : 변경이 즉시 적용됨

TCSRADRAIN: 변경이 fd에 쓰기모든 출력이 단말에 송신된 다음에 행해진다 --> 출력에 영향을 주는 파라미터를 변경할때 사용

TCSAFLUSH: '', But 수신되었지만 아직 읽어내지않은것을 버린다

TCSASOFT: c_cflag, c_ispeed 및 c_ospeed 필드의 값은 무시
*/

void reset_terminal_mode() {
    tcsetattr(0, TCSANOW, &orig_termios);
}

void set_conio_terminal_mode() {
    struct termios save_termios;

    // 기본적으로 termios 구조의 취득 및 설절을 위한 것.h>
    // Take two copies, one for now and one for later
    tcgetattr(0, &orig_termios);
    memcpy(&save_termios, &orig_termios, sizeof(save_termios));

    // Register cleanup handler, and set the new terminal mode
    atexit(reset_terminal_mode);
    // cfmakeraw 는 termios 구조에 보존된 플래그를 모든 입력 및 출력 처리를 무효로 하는상태로 만든다.
    cfmakeraw(&save_termios);
    // 기본적으로 스냅샷과 같은 동작을 취한다.
    
    tcsetattr(0, TCSANOW, &save_termios);
}

int keyboard_hit(int fs) {
    struct timeval tv = { 0L, 0L };
    int maxndfs = fs > 1 ? fs : 1;
    fd_set fds;
    
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    return select(maxndfs, &fds, NULL, NULL, &tv);
}

int get_char(int fs) {
    int r;
    unsigned char c;

    if(fs == 0){
        if((r = read(fs, &c, sizeof(c))) < 0)
            return r;
        else
            return c;
    }else{
        if((r = recv(fs, &c, sizeof(c), 0)) < 0)
            return r;
        else
            return c;
    }

}
