/******************************************************************************
*******************************************************************************
*******************************************************************************

    Copyright (C) 2014 Ben Martin

    This is an example of using the SFE part "Blackberry Trackballer Breakout"

    This is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software.  If not, see <http://www.gnu.org/licenses/>.

    For more details see the COPYING file in the root directory of this
    distribution.

*******************************************************************************
*******************************************************************************
******************************************************************************/


#include <Streaming.h>
#include <Wire.h>
#include <SwitchMCP.h>
#include <SimpleTimer.h>

SimpleTimer timer;


const byte I2CADDR_MCP27017 = 0x20;

MCPReader reader( I2CADDR_MCP27017 );
MCPReader readerb( I2CADDR_MCP27017, MCPReader::mcpPORTB );


int redCurrent = 0;
int redChange = 4;
int redPin = 5;

void setup()
{
    Serial.begin(115200);
    
    Wire.begin();
    reader.init();
    readerb.init();
    
    pinMode(redPin,   OUTPUT);   // sets the pins as output
    pinMode(2, INPUT );
    pinMode(3, OUTPUT );

    // reset the state to all zeros.
    digitalWrite( 3, 1 );
    delay(100);
    digitalWrite( 3, 0 );
}

void rezero()
{
    digitalWrite( 3, 1 );
    delay(1);
    digitalWrite( 3, 0 );
}

int updownmax=10;
char* updown[] = { "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", 0 };
int updownidx = 5;


///////////
///////////
///////////

// This timer is used by calling touchScreenRefresherTimer()
// when the up and down events happen. After a small delay the
// current choice is "locked in" using ScreenRefresherTimer()

int TimerNull = 50;
int m_screenRefreshTimerID = TimerNull;

void ScreenRefresherTimer()
{
  Serial << " *********************** activate:" << updown[updownidx] << endl;
  m_screenRefreshTimerID = TimerNull;
}
void
touchScreenRefresherTimer()
{
    if( m_screenRefreshTimerID != TimerNull )
        timer.restartTimer(m_screenRefreshTimerID);
    else
        m_screenRefreshTimerID = timer.setTimeout( 1000, ScreenRefresherTimer );
    
//    Serial << "timerID:" << m_screenRefreshTimerID << endl;
}



///////////
///////////
///////////

void loop()
{
  delay(100);
  byte b = reader.poll(); 
  byte bb = readerb.poll(); 
  b = ~b;
  bb = ~bb;
  int l = b & 0xF;
  int r = (b >> 4) & 0xF;
  int u = bb & 0xF;
  int d = (bb >> 4) & 0xF;
  r-=15;
  d-=15;

  // calling rezero will flash the MR lines on all 393 ripple counters
  // which will reset them. This way, we only get what has "changed"
  // since the last time we read the 393 values.
  rezero();  
  Serial //<< " b:" << b << " bb:" << bb << "   " 
         << " l:" << l << " r:" << r << " u:" << u << " d:" << d << " ";

  // A small example allowing walking a list using up and down
  // rolling, after a period of inactivity the selection is locked
  // in. For example, when scrolling a list on a screen might be quick
  // but acting on the selection might take time which would not
  // work well for keeping the user happy while they roll the ball.
  int old = updownidx;
  updownidx += u;
  updownidx += d;
  updownidx = max( updownidx, 0 );
  updownidx = min( updownidx, updownmax );
  Serial << updown[updownidx];
  Serial << endl;
  if( updownidx != old )
      touchScreenRefresherTimer();  
  timer.run();


  // This is the trivial up and down on the red led just
  // because I felt like it.
  redCurrent += redChange;
  if( !redCurrent )
    redChange *= -1;
  if( redCurrent >= 250 )
    redChange *= -1;
  analogWrite(redPin,   redCurrent);  
}

