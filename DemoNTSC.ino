#include <TVout.h>
#include <video_gen.h>
#include <pollserial.h>

#include "fontALL.h"
#include "schematic.h"
#include "gc.h"
#include "mtn.h"
#include "TVOlogo.h"

String inputed;
TVout TV;
pollserial pserial;

int zOff = 150;
int xOff = 0;
int yOff = 0;
int cSize = 50;
int view_plane = 64;
float angle = PI / 60;

float cube3d[8][3] = {
    {xOff - cSize, yOff + cSize, zOff - cSize},
    {xOff + cSize, yOff + cSize, zOff - cSize},
    {xOff - cSize, yOff - cSize, zOff - cSize},
    {xOff + cSize, yOff - cSize, zOff - cSize},
    {xOff - cSize, yOff + cSize, zOff + cSize},
    {xOff + cSize, yOff + cSize, zOff + cSize},
    {xOff - cSize, yOff - cSize, zOff + cSize},
    {xOff + cSize, yOff - cSize, zOff + cSize}};
unsigned char cube2d[8][2];

void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
  digitalWrite(LED_BUILTIN,LOW);
  TV.begin(NTSC, 120, 96);
  TV.select_font(font6x8);
  TV.set_hbi_hook(pserial.begin(9600));

  //intro();
  TV.println("Happy Fathers Day,\n    Dad!\n");
  TV.delay(250);
  TV.println("\n        Love, Ben");
  TV.delay(5000);
  TV.clear_screen();
  TV.bitmap(0, 0, mtns);
  TV.delay(10000);
  TV.clear_screen();
  TV.bitmap(0, 0, GClogo);
  TV.delay(10000);
  TV.clear_screen();
  digitalWrite(LED_BUILTIN,HIGH);
}




void loop() {
  if (pserial.available()) {
    int key_id = pserial.read();
    char inp =  (char)key_id;
    TV.print(inp);
    //TV.print(key_id);
    inputed += inp;
    if (inp == '\r') {
      TV.println();
      if (inputed == "mtn\r") {
        TV.clear_screen();
        TV.bitmap(0, 0, mtns);
        TV.delay(20000);
        TV.clear_screen();
      } else if (inputed == "intro\r") {
        TV.clear_screen();
        intro();
        TV.delay(500);
        TV.clear_screen();
      } else if (inputed == "clr\r") {
        TV.clear_screen();
      } else if (inputed == "schem\r") {
        TV.clear_screen();
        TV.bitmap(0, 0, schematic);
        TV.delay(5000);
        TV.clear_screen();
      }
      

      inputed = "";
    }
  }
}

void intro()
{
  unsigned char w, l, wb;
  int index;
  w = pgm_read_byte(TVOlogo);
  l = pgm_read_byte(TVOlogo + 1);
  if (w & 7)
    wb = w / 8 + 1;
  else
    wb = w / 8;
  index = wb * (l - 1) + 2;
  for (unsigned char i = 1; i < l; i++)
  {
    TV.bitmap((TV.hres() - w) / 2, 0, TVOlogo, index, w, i);
    index -= wb;
    TV.delay(50);
  }
  for (unsigned char i = 0; i < (TV.vres() - l) / 2; i++)
  {
    TV.bitmap((TV.hres() - w) / 2, i, TVOlogo);
    TV.delay(50);
  }
  TV.delay(3000);
  TV.clear_screen();
}

void printcube()
{
  // calculate 2d points
  for (byte i = 0; i < 8; i++)
  {
    cube2d[i][0] = (unsigned char)((cube3d[i][0] * view_plane / cube3d[i][2]) + (TV.hres() / 2));
    cube2d[i][1] = (unsigned char)((cube3d[i][1] * view_plane / cube3d[i][2]) + (TV.vres() / 2));
  }
  TV.delay_frame(1);
  TV.clear_screen();
  draw_cube();
}

void zrotate(float q)
{
  float tx, ty, temp;
  for (byte i = 0; i < 8; i++)
  {
    tx = cube3d[i][0] - xOff;
    ty = cube3d[i][1] - yOff;
    temp = tx * cos(q) - ty * sin(q);
    ty = tx * sin(q) + ty * cos(q);
    tx = temp;
    cube3d[i][0] = tx + xOff;
    cube3d[i][1] = ty + yOff;
  }
}

void yrotate(float q)
{
  float tx, tz, temp;
  for (byte i = 0; i < 8; i++)
  {
    tx = cube3d[i][0] - xOff;
    tz = cube3d[i][2] - zOff;
    temp = tz * cos(q) - tx * sin(q);
    tx = tz * sin(q) + tx * cos(q);
    tz = temp;
    cube3d[i][0] = tx + xOff;
    cube3d[i][2] = tz + zOff;
  }
}

void xrotate(float q)
{
  float ty, tz, temp;
  for (byte i = 0; i < 8; i++)
  {
    ty = cube3d[i][1] - yOff;
    tz = cube3d[i][2] - zOff;
    temp = ty * cos(q) - tz * sin(q);
    tz = ty * sin(q) + tz * cos(q);
    ty = temp;
    cube3d[i][1] = ty + yOff;
    cube3d[i][2] = tz + zOff;
  }
}

void draw_cube()
{
  TV.draw_line(cube2d[0][0], cube2d[0][1], cube2d[1][0], cube2d[1][1], WHITE);
  TV.draw_line(cube2d[0][0], cube2d[0][1], cube2d[2][0], cube2d[2][1], WHITE);
  TV.draw_line(cube2d[0][0], cube2d[0][1], cube2d[4][0], cube2d[4][1], WHITE);
  TV.draw_line(cube2d[1][0], cube2d[1][1], cube2d[5][0], cube2d[5][1], WHITE);
  TV.draw_line(cube2d[1][0], cube2d[1][1], cube2d[3][0], cube2d[3][1], WHITE);
  TV.draw_line(cube2d[2][0], cube2d[2][1], cube2d[6][0], cube2d[6][1], WHITE);
  TV.draw_line(cube2d[2][0], cube2d[2][1], cube2d[3][0], cube2d[3][1], WHITE);
  TV.draw_line(cube2d[4][0], cube2d[4][1], cube2d[6][0], cube2d[6][1], WHITE);
  TV.draw_line(cube2d[4][0], cube2d[4][1], cube2d[5][0], cube2d[5][1], WHITE);
  TV.draw_line(cube2d[7][0], cube2d[7][1], cube2d[6][0], cube2d[6][1], WHITE);
  TV.draw_line(cube2d[7][0], cube2d[7][1], cube2d[3][0], cube2d[3][1], WHITE);
  TV.draw_line(cube2d[7][0], cube2d[7][1], cube2d[5][0], cube2d[5][1], WHITE);
}
