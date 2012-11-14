
void putch( char ch );

void initUsart();
char usartReady();
unsigned char usartGetByte();

void usartRCUpdate();
void usartTXUpdate();
void usartTask();
