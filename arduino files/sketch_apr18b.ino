#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyArduino.h>
#include <SPI.h>

#define TFT_CS 5
#define TFT_RST 4
#define TFT_DC 2
#define TFT_SCLK 6
#define TFT_MOSI 7
#define BTN_NEXT 8
#define BTN_PREV 10
#define BTN_PLAY 3
#define BTN_VOL_UP 9
#define BTN_VOL_DOWN 20
#define BTN_EXTRA 21

char* SSID = "YOUR WIFI SSID";
const char* PASSWORD = "YOUR WIFI PASSWORD";
const char* CLIENT_ID = "Client ID Here";
const char* CLIENT_SECRET = "Client secret here";

String lastArtist;
String lastTrackname;
String currentArtist;
String currentTrackname;

int progress = 0;
int duration = 1;
int currentVolume = 50; 

WiFiClient spotifyClient;

SpotifyArduino MANI(spotifyClient, CLIENT_ID, CLIENT_SECRET, "YOUR_REFRESH_TOKEN_HERE");
Adafruit_ST7735 ROSE = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

String truncateText(String text, int maxLength) {
    if (text.length() > maxLength) {
        return text.substring(0, maxLength) + "...";
    }
    return text;
}

void drawProgressBar(int progress, int duration) {
    int barWidth = 120;
    int barHeight = 8;
    int x = 10;
    int y = 100;

    int filled = (progress * barWidth) / duration;

    ROSE.drawRect(x, y, barWidth, barHeight, ST77XX_WHITE);
    ROSE.fillRect(x, y, filled, barHeight, ST77XX_RED);
}

void drawVolumeBar(int volume) {
    int barWidth = 120;
    int barHeight = 6;
    int x = 10;
    int y = 115;
    
    int filled = (volume * barWidth) / 100;
    
    ROSE.drawRect(x, y, barWidth, barHeight, ST77XX_WHITE);
    ROSE.fillRect(x, y, filled, barHeight, ST77XX_GREEN);
}

String animateCapitalLetters(String text, int step) {
    String result = "";
    for (int i = 0; i < text.length(); i++) {
        if (i <= step % (text.length() + 1)) {
            // makes letter capital
            result += (char)toupper(text[i]);
        } else {
            // keep original case
            result += text[i];
        }
    }
    return result;
}

void drawTimeCounter(int progress, int duration) {
    // Calculate minutes and seconds for current position
    int currentSeconds = progress / 1000;
    int currentMinutes = currentSeconds / 60;
    int currentRemainingSeconds = currentSeconds % 60;
    
    // Calculate total song length
    int totalSeconds = duration / 1000;
    int totalMinutes = totalSeconds / 60;
    int totalRemainingSeconds = totalSeconds % 60;
    
    // Format as "0*:** / 0*:**"
    char timeText[30];
    sprintf(timeText, "%02d:%02d / %02d:%02d", 
            currentMinutes, currentRemainingSeconds,
            totalMinutes, totalRemainingSeconds);
    
    // display time below progress bar
    ROSE.setTextSize(1);
    ROSE.setTextColor(ST77XX_RED);
    ROSE.setCursor(10, 112);  // below progress bar (y=100 + 8 height + 4 gap)
    ROSE.print(timeText);
}

void drawVolumePercent(int volume) {
    // display volume as percentage
    char volumeText[10];
    sprintf(volumeText, "Vol: %d%%", volume);
    
    ROSE.setTextSize(1);
    ROSE.setTextColor(0xFFE0);
    ROSE.setCursor(120, 115);  
    ROSE.print(volumeText);
}

void drawPakistanFlag() {
    ROSE.fillRect(140, 0, 19, 15, 0x07E0);
    // MOON
    ROSE.fillCircle(149, 7, 4, ST77XX_WHITE);
    ROSE.fillCircle(147, 7, 3, 0x07E0);  
    // star ig
    ROSE.fillRect(153, 6, 2, 2, ST77XX_WHITE);
    ROSE.fillRect(154, 5, 1, 1, ST77XX_WHITE);
    ROSE.fillRect(154, 8, 1, 1, ST77XX_WHITE);
}

void currentlyPlayingCallback(CurrentlyPlaying currentlyPlaying) {
    // Print EVERYTHING to see what's inside
    Serial.println("=== TEST ===");
    
    // Try different possibilities
    Serial.println("Artist test: " + String(currentlyPlaying.artists[0].name));
    Serial.println("Artist name test: " + currentlyPlaying.artists[0].name);
    
    // Print raw data if possible
    Serial.println("Raw artists: " + currentlyPlaying.artists[0]);
    
    Serial.println("=== END TEST ===");
}

void setup() {
    Serial.begin(115200);

    pinMode(BTN_NEXT, INPUT_PULLUP);
    pinMode(BTN_PREV, INPUT_PULLUP);
    pinMode(BTN_PLAY, INPUT_PULLUP);
    pinMode(BTN_VOL_UP, INPUT_PULLUP);
    pinMode(BTN_VOL_DOWN, INPUT_PULLUP);
    pinMode(BTN_EXTRA, INPUT_PULLUP);

    ROSE.initR(INITR_BLACKTAB); // the type of screen
    ROSE.setRotation(1); // this makes the screen landscape! remove this line for portrait
    Serial.println("TFT Initialized!");
    // STARTUP ANIMATION WITH THUMPING HEART
    ROSE.fillScreen(ST77XX_BLACK);

    // heart that thumps 8 times
    for (int thump = 0; thump < 8; thump++) {
        ROSE.fillScreen(ST77XX_BLACK);  // clear screen each thump
    
        // heart gets bigger and smaller (thumping effect)
        int heartSize;
        if (thump % 2 == 0) {
            heartSize = 20;  // normal size on even thumps
        } else {
            heartSize = 28;  // bigger size on odd thumps 
        }
    
        // Draw the heart in center of screen
        int centerX = 80;
        int centerY = 50;   
    
        
        ROSE.fillCircle(centerX - heartSize/2, centerY, heartSize/2, ST77XX_RED);
        ROSE.fillCircle(centerX + heartSize/2, centerY, heartSize/2, ST77XX_RED);
    
        
        for (int y = centerY; y <= centerY + heartSize; y++) {
            int width = (y - centerY) * 2;
            ROSE.drawLine(centerX - width/2, y, centerX + width/2, y, ST77XX_RED);
        }
    
        ROSE.setTextColor(0xF81F);
        ROSE.setTextSize(2);
        ROSE.setCursor(25, 85);
        ROSE.print("Manis Music");
    
        ROSE.setTextSize(1);
        ROSE.setCursor(55, 105);
        ROSE.print("By The Man");
    
        delay(300);
    }

    // show final heart with loading text
    ROSE.fillScreen(ST77XX_BLACK);
    // draw final heart at normal size
    int centerX = 80;
    int centerY = 50;
    ROSE.fillCircle(centerX - 10, centerY, 10, ST77XX_RED);
    ROSE.fillCircle(centerX + 10, centerY, 10, ST77XX_RED);
    for (int y = centerY; y <= centerY + 20; y++) {
        int width = (y - centerY) * 2;
        ROSE.drawLine(centerX - width/2, y, centerX + width/2, y, ST77XX_RED);
    }

    ROSE.setTextColor(0xF81F);
    ROSE.setTextSize(2);
    ROSE.setCursor(20, 85);
    ROSE.print("Manis Music");
    ROSE.setTextSize(1);
    ROSE.setCursor(55, 105);
    ROSE.print("By The Man");

    delay(1000);
    ROSE.fillScreen(ST77XX_BLACK);
    ROSE.setCursor(20, 50);
    ROSE.setTextColor(ST77XX_GREEN);
    ROSE.setTextSize(2);
    ROSE.print("Loading...");
    delay(1500);
    ROSE.fillScreen(ST77XX_BLACK);
    // END OF STARTUP ANIMATION

    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi...");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }
    Serial.printf("\nConnected!\n");

    ROSE.setCursor(0,0); // make the cursor at the top left
    ROSE.write(WiFi.localIP().toString().c_str()); // print out IP on the screen

}

void loop()
{
    int rainbowColors[] = {ST77XX_CYAN, ST77XX_MAGENTA, ST77XX_YELLOW, ST77XX_GREEN, 0xFD20, 0xF81F, 0x07FF};
    static int colorCycle = 0;  

    if (digitalRead(BTN_NEXT) == LOW) {
       MANI.nextTrack();
       delay(300);
    }

    if (digitalRead(BTN_PREV) == LOW) {
        MANI.previousTrack();
        delay(300);
    }

    if (digitalRead(BTN_PLAY) == LOW) {
        MANI.play();
        delay(300);
    }

    if (digitalRead(BTN_VOL_UP) == LOW) {
    currentVolume += 5;
    if (currentVolume > 100) currentVolume = 100;
    MANI.setVolume(currentVolume);
    delay(200);
    }

    if (digitalRead(BTN_VOL_DOWN) == LOW) {
    currentVolume -= 5;
    if (currentVolume < 0) currentVolume = 0;
    MANI.setVolume(currentVolume);
    delay(200);
    }

    MANI.getCurrentlyPlaying(currentlyPlayingCallback);

    drawProgressBar(progress, duration);
    drawTimeCounter(progress, duration);
    drawVolumeBar(currentVolume);  
    drawVolumePercent(currentVolume);
    drawPakistanFlag();

    static int bounceY = 0;
    static int bounceDirection = 1;

    ROSE.fillRect(10, 0, 10, 30, ST77XX_BLACK);
    ROSE.setCursor(10, 10 + bounceY);
    ROSE.setTextColor(ST77XX_GREEN);
    ROSE.print("♪");

    bounceY += bounceDirection;
    if (bounceY > 10 || bounceY < 0) bounceDirection = -bounceDirection;

    static int animationStep = 0;
    String animatedArtist = animateCapitalLetters(currentArtist, animationStep);
    String animatedTrack = animateCapitalLetters(currentTrackname, animationStep);

    if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {
        lastArtist = currentArtist;
        Serial.println("Artist: " + lastArtist);
        animationStep = 0;  // reset animation when song changes
    }
    ROSE.fillRect(0, 0, 160, 30, ST77XX_BLACK);
    // Draw shadow 
    ROSE.setCursor(12, 12);
    ROSE.setTextColor(0x1082); 
    ROSE.write(animatedArtist.c_str());
    // draws the main text
    ROSE.setCursor(10, 10);
    ROSE.setTextColor(rainbowColors[colorCycle % 7]);
    ROSE.write(animatedArtist.c_str());

    if (lastTrackname != currentTrackname && currentTrackname != "Something went wrong" && currentTrackname != "null") {
        lastTrackname = currentTrackname;
        Serial.println("Track: " + lastTrackname);
        animationStep = 0;  // reset animation when song changes
    }
    ROSE.fillRect(0, 30, 160, 30, ST77XX_BLACK);
    // Draw shadow
    ROSE.setCursor(12, 42);
    ROSE.setTextColor(0x1082);
    ROSE.write(animatedTrack.c_str());
    // Draw main text
    ROSE.setCursor(10, 40);
    ROSE.setTextColor(rainbowColors[colorCycle % 7]);
    ROSE.write(animatedTrack.c_str());
    animationStep++;  // move to next letter    

    drawProgressBar(progress, duration);
    drawTimeCounter(progress, duration);
    drawVolumeBar(currentVolume);  
    drawVolumePercent(currentVolume);
    drawPakistanFlag();

    colorCycle++;

    delay(1000);
}