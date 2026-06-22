#include <SpotifyArduinoCert.h>
#include <WiFiClientSecure.h>

#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <WiFi.h>
#include <SpotifyArduino.h>
#include <SPI.h>

#define TFT_CS 4
#define TFT_RST 2
#define TFT_DC 3
#define TFT_SCLK 6
#define TFT_MOSI 7
#define BTN_NEXT 0
#define BTN_PREV 10
#define BTN_PLAY 1
#define BTN_VOL_UP 8
#define BTN_VOL_DOWN 5

const char* SSID = "AhmedAli2.4G_EXT";
const char* PASSWORD = "Ishfaque";
const char* CLIENT_ID = "efc9b3323f5c423f9299b7453264254a";
const char* CLIENT_SECRET = "9a4407aeb57e486c855c03857f4ae774";

unsigned long lastSpotifyUpdate = 0;
const int spotifyInterval = 4000;

unsigned long lastLocalUpdate = 0;
int localProgress = 0;
bool isPlaying = false;

String lastArtist;  
String lastTrackname;
String currentArtist;
String currentTrackname;

int progress = 0;
int duration = 1;
int currentVolume = 50; 

int rainbowColors[] = {ST77XX_CYAN, ST77XX_MAGENTA, ST77XX_YELLOW, ST77XX_GREEN, 0xFD20, 0xF81F, 0x07FF};
int animationStep = 0;

WiFiClientSecure spotifyClient;

SpotifyArduino MANI(spotifyClient, CLIENT_ID, CLIENT_SECRET);
Adafruit_ST7735 ROSE = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

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
    int y = 60;
    ROSE.fillRect(x, y, barWidth, barHeight, ST77XX_BLACK);
    int filled = (progress * barWidth) / duration;

    ROSE.drawRect(x, y, barWidth, barHeight, ST77XX_WHITE);
    ROSE.fillRect(x, y, filled, barHeight, ST77XX_RED);
}

void drawVolumeBar(int volume) {
    int barWidth = 120;
    int barHeight = 6;
    int x = 10;
    int y = 95;
    ROSE.fillRect(x, y, barWidth, barHeight, ST77XX_BLACK);
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
    ROSE.fillRect(10, 70, 140, 20, ST77XX_BLACK);
    ROSE.setCursor(10, 72);
    ROSE.print(timeText);
}

void drawVolumePercent(int volume) {
    char volumeText[12];
    sprintf(volumeText, "Vol: %d%%", volume);

    ROSE.fillRect(10, 108, 80, 14, ST77XX_BLACK);
    ROSE.setTextSize(1);
    ROSE.setTextColor(0xFFE0);
    ROSE.setCursor(10, 110);
    ROSE.print(volumeText);
}

void drawPakistanFlag() {

    int x = 130;
    int y = 2;
    int w = 25;
    int h = 16;

    // LEFT WHITE STRIPE
    ROSE.fillRect(x, y, 5, h, ST77XX_WHITE);

    // RIGHT GREEN PART
    ROSE.fillRect(x + 5, y, w - 5, h, 0x07E0);

    // MOON
    ROSE.fillCircle(x + 14, y + 8, 4, ST77XX_WHITE);
    ROSE.fillCircle(x + 15, y + 8, 3, 0x07E0);

    // STAR (simple dot version for reliability)
    ROSE.fillRect(x + 18, y + 6, 2, 2, ST77XX_WHITE);
}

void currentlyPlayingCallback(CurrentlyPlaying currentlyPlaying) {

    if (currentlyPlaying.trackName == "") {
        isPlaying = false;
        return;
    }

    isPlaying = currentlyPlaying.isPlaying;

    currentArtist = currentlyPlaying.artists[0].artistName;
    currentTrackname = currentlyPlaying.trackName;

    duration = currentlyPlaying.durationMs;

    // ALWAYS sync progress from Spotify
    progress = currentlyPlaying.progressMs;

    // smooth local timer continues from spotify value
    localProgress = progress;

    lastLocalUpdate = millis();

    // detect new song ONLY for animations
    static String prevTrack = "";

    if (currentTrackname != prevTrack) {

        Serial.println("NEW SONG: " + currentTrackname);

        animationStep = 0;

        prevTrack = currentTrackname;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    pinMode(BTN_NEXT, INPUT_PULLUP);
    pinMode(BTN_PREV, INPUT_PULLUP);
    pinMode(BTN_PLAY, INPUT_PULLUP);
    pinMode(BTN_VOL_UP, INPUT_PULLUP);
    pinMode(BTN_VOL_DOWN, INPUT_PULLUP);

    Serial.println("1");



    SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);

    ROSE.initR(INITR_BLACKTAB); // the type of screen
    Serial.println("2");
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
            heartSize = 22;  // normal size on even thumps
        } else {
            heartSize = 28;  // bigger size on odd thumps 
        }
    
        // Draw the heart in center of screen
        int centerX = 80;
        int centerY = 50;   
    
        
        ROSE.fillCircle(centerX - heartSize/2, centerY, heartSize/2, ST77XX_RED);
        ROSE.fillCircle(centerX + heartSize/2, centerY, heartSize/2, ST77XX_RED);
    
        
        for (int y = 0; y <= heartSize; y++) {

            // make the heart get narrower toward bottom
            int width = (heartSize*2) - (y*2);

            ROSE.drawLine(
                centerX - width/2,
                centerY + y + 4,

                centerX + width/2,
                centerY + y + 4,

                ST77XX_RED
            );
        }
    
        ROSE.setTextColor(0xF81F);
        ROSE.setTextSize(2);
        ROSE.setCursor(25, 85);
        ROSE.print("Manis Music");
    
        ROSE.setTextSize(1);
        ROSE.setCursor(55, 105);
        ROSE.print("By The Man");
    
        delay(800);
    }

    // show final heart with loading text
    ROSE.fillScreen(ST77XX_BLACK);
    // draw final heart at normal size
    int centerX = 80;
    int centerY = 50;
    ROSE.fillCircle(centerX - 10, centerY, 10, ST77XX_RED);
    ROSE.fillCircle(centerX + 10, centerY, 10, ST77XX_RED);
        for (int y = 0; y <= 20; y++) {

            // starts wide, shrinks downward
            int width = 40 - (y * 2);

            ROSE.drawLine(
                centerX - width/2,
                centerY + y + 4,
               centerX + width/2,
                centerY + y + 4,
                ST77XX_RED
            );
        }

    ROSE.setTextColor(0xF81F);
    ROSE.setTextSize(2);
    ROSE.setCursor(20, 85);
    ROSE.print("Manis Music");
    ROSE.setTextSize(1);
    ROSE.setCursor(55, 105);
    ROSE.print("By The Man");

    delay(5000);
    ROSE.fillScreen(ST77XX_BLACK);
    ROSE.setCursor(20, 50);
    ROSE.setTextColor(ST77XX_GREEN);
    ROSE.setTextSize(2);
    ROSE.print("Loading...");
    delay(1500);
    ROSE.fillScreen(ST77XX_BLACK);
    // END OF STARTUP ANIMATION

    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    delay(500);

    Serial.print("Connecting to ");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    unsigned long startAttempt = millis();

        while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 45000) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("RSSI: ");
        Serial.println(WiFi.RSSI());
    } else {
        Serial.print("WiFi failed. Status: ");
        Serial.println(WiFi.status());

        ROSE.fillScreen(ST77XX_BLACK);
        ROSE.setCursor(10, 50);
        ROSE.setTextColor(ST77XX_RED);
        ROSE.setTextSize(1);
        ROSE.print("WiFi failed: ");
        ROSE.print(WiFi.status());

        return;
    }

    Serial.println("3");

    spotifyClient.setInsecure();
    spotifyClient.setTimeout(15000);
    delay(1000);
    IPAddress ip;

    if (WiFi.hostByName("accounts.spotify.com", ip)) {
        Serial.print("Spotify IP: ");
        Serial.println(ip);
    }
    else {
    Serial.println("DNS FAILED");
    }

    MANI.setRefreshToken("AQAGvEcU3IFutksX-ZwhuGU5HzzydMnZNU3SmOzUyqz8AdO760G7xFRiH_0ByRsRmENwqXhinFLC65DGcae4VV2JLmmHvr7gfEVsJGz9uGDUJky3wh-8eaTfu-cE5hizhGg");
    MANI.refreshAccessToken();

    Serial.println("4");

    MANI.getCurrentlyPlaying(currentlyPlayingCallback);

    ROSE.setCursor(0,0); // make the cursor at the top left
    ROSE.write(WiFi.localIP().toString().c_str()); // print out IP on the screen
    ROSE.fillScreen(ST77XX_BLACK);

}

void loop()
{
    static int colorCycle = 0;  

    if (digitalRead(BTN_NEXT) == LOW) {
        Serial.println("NEXT button pressed");

        bool ok = MANI.nextTrack();
        Serial.print("nextTrack result: ");
        Serial.println(ok);

        delay(700);
        MANI.getCurrentlyPlaying(currentlyPlayingCallback);
        lastSpotifyUpdate = millis();
    }

    if (digitalRead(BTN_PREV) == LOW) {
        Serial.println("PREV button pressed");

        bool ok = MANI.previousTrack();
        Serial.print("previousTrack result: ");
        Serial.println(ok);

        delay(700);
        MANI.getCurrentlyPlaying(currentlyPlayingCallback);
        lastSpotifyUpdate = millis();
    }

    if (digitalRead(BTN_PLAY) == LOW) {
        Serial.println("PLAY button pressed");

        bool ok;

        if (isPlaying) {
            ok = MANI.pause();
            Serial.print("pause result: ");
            Serial.println(ok);
            if (ok) isPlaying = false;
        } else {
                ok = MANI.play();
                Serial.print("play result: ");
                Serial.println(ok);
                if (ok) isPlaying = true;
        }

        delay(700);
        MANI.getCurrentlyPlaying(currentlyPlayingCallback);
        lastSpotifyUpdate = millis();
    }

    if (digitalRead(BTN_VOL_UP) == LOW) {
        Serial.println("VOL UP button pressed");

        currentVolume += 5;
        if (currentVolume > 100) currentVolume = 100;

        drawVolumeBar(currentVolume);
        drawVolumePercent(currentVolume);

        bool ok = MANI.setVolume(currentVolume);
        Serial.print("setVolume up result: ");
        Serial.println(ok);
        Serial.print("requested volume: ");
        Serial.println(currentVolume);

        delay(700);
    }

    if (digitalRead(BTN_VOL_DOWN) == LOW) {
        Serial.println("VOL DOWN button pressed");

        currentVolume -= 5;
        if (currentVolume < 0) currentVolume = 0;

        drawVolumeBar(currentVolume);
        drawVolumePercent(currentVolume);

        bool ok = MANI.setVolume(currentVolume);
        Serial.print("setVolume down result: ");
        Serial.println(ok);
        Serial.print("requested volume: ");
        Serial.println(currentVolume);

        delay(700);
    }

   if (WiFi.status() != WL_CONNECTED) {
        isPlaying = false;
    }

    if (millis() - lastSpotifyUpdate > spotifyInterval) {

        if (WiFi.status() == WL_CONNECTED) {

            bool ok = MANI.getCurrentlyPlaying(currentlyPlayingCallback);

            if (!ok) {
                Serial.println("Spotify fetch failed");
            }

        }

        lastSpotifyUpdate = millis();
    }

    if (isPlaying) {

        unsigned long now = millis();

        localProgress += (now - lastLocalUpdate);
        lastLocalUpdate = now;

        if (localProgress > duration) {
            localProgress = duration;
        }

        progress = localProgress;
    }

    ROSE.fillRect(0, 0, 120, 30, ST77XX_BLACK);   // artist area only
    ROSE.fillRect(0, 30, 160, 25, ST77XX_BLACK);  // track area

    String animatedArtist = animateCapitalLetters(currentArtist, animationStep);
    String animatedTrack = animateCapitalLetters(currentTrackname, animationStep);

    drawProgressBar(localProgress, duration);
    drawTimeCounter(localProgress, duration);
    drawVolumeBar(currentVolume);  
    drawVolumePercent(currentVolume);
    drawPakistanFlag();

    if (lastArtist != currentArtist && currentArtist != "Something went wrong" && !currentArtist.isEmpty()) {
        lastArtist = currentArtist;
        Serial.println("Artist: " + lastArtist);
        animationStep = 0;  // reset animation when song changes
    }
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
    // Draw shadow
    ROSE.setCursor(12, 42);
    ROSE.setTextColor(0x1082);
    ROSE.write(animatedTrack.c_str());
    // Draw main text
    ROSE.setCursor(10, 40);
    ROSE.setTextColor(rainbowColors[colorCycle % 7]);
    ROSE.write(animatedTrack.c_str());
    animationStep++;  // move to next letter    
    colorCycle++;

    delay(30);
}