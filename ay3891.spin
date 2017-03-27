{{
┌──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                          AYcog - AY3891X / YM2149F emulator V0.8 (C) 2012 Johannes Ahlebrand                                 │
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                    TERMS OF USE: Parallax Object Exchange License                                            │                                                            
├──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┤
│Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation    │ 
│files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,    │
│modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software│
│is furnished to do so, subject to the following conditions:                                                                   │
│                                                                                                                              │
│The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.│
│                                                                                                                              │
│THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE          │
│WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR         │
│COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,   │
│ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                         │
└──────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────┘

Revision History

v0.6 (April 2011)    - Initial OBEX release
                     - Added convenient API methods

v0.8 (May 2012)      - Increased volume table accuracy
                     - Increased noise accuracy  
                     - Opmtimized code a little bit

}}

CON

 ' WARNING !!
 ' Don't alter the constants below unless you know what you are doing
 '-------------------------------------------------------------------
  PSG_FREQ    = 2_000_000.0              ' Clock frequency input on the emulated AY chip
  SAMPLE_RATE = 125_000                  ' Sample rate of AYcog (125 kHz is maximum for an 80 Mhz propeller)
   
  OSC_CORR    = trunc(1.05 * PSG_FREQ)   ' Relative oscillator frequency
  NOISE_CORR  = OSC_CORR>>1              ' Relative noise frequency
  ENV_CORR    = OSC_CORR>>5              ' Relative envelope timing

  NOISE_TAP = %1001                      ' LFSR noise constants
  NOISE_MSB = 17                         '

  VOLUME_CORRECTION = 0.7                ' Volume correction value     
  MAX_AMPLITUDE  = float(POSX / 4)       ' maxInt32value / numberOfChannels (this makes room for maximum "swing" on all channels) 
  AMPLITUDE_DAMP_FACTOR = 0.75           ' The damp factor between each volume step     

  AMPLITUDE_LEVEL_F = MAX_AMPLITUDE     * VOLUME_CORRECTION
  AMPLITUDE_LEVEL_E = AMPLITUDE_LEVEL_F * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_D = AMPLITUDE_LEVEL_E * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_C = AMPLITUDE_LEVEL_D * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_B = AMPLITUDE_LEVEL_C * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_A = AMPLITUDE_LEVEL_B * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_9 = AMPLITUDE_LEVEL_A * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_8 = AMPLITUDE_LEVEL_9 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_7 = AMPLITUDE_LEVEL_8 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_6 = AMPLITUDE_LEVEL_7 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_5 = AMPLITUDE_LEVEL_6 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_4 = AMPLITUDE_LEVEL_5 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_3 = AMPLITUDE_LEVEL_4 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_2 = AMPLITUDE_LEVEL_3 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_1 = AMPLITUDE_LEVEL_2 * AMPLITUDE_DAMP_FACTOR
  AMPLITUDE_LEVEL_0 = 0.0

PUB start(right, left)
' ┌──────────────────────────────────────────────────────────────┐
' │                Starts AYcog in a single cog                  │
' ├──────────────────────────────────────────────────────────────┤
' │ Returns a pointer to the first AY register in hub memory     │
' │ on success; otherwise returns 0.                             │
' │                                                              │ 
' │ right - The pin to output the right channel to. 0 = Not used │
' │                                                              │
' │ left - The pin to output the left channel to. 0 = Not used   │
' └──────────────────────────────────────────────────────────────┘
  arg1 := $18000000 | left
  arg2 := $18000000 | right
  r1 := ((1<<right) | (1<<left))&!1
  sampleRate := clkfreq/SAMPLE_RATE
  cog := cognew(@AYEMU, @AYregisters) + 1

  if cog
    return @AYregisters
  else
    return 0

PUB stop
' ┌──────────────────────────────────────────────────────────────┐
' │                         Stops AYcog                          │
' └──────────────────────────────────────────────────────────────┘
  if cog
    cogstop(cog~ -1)
    cog := 0
    
PUB setRegister(reg, val) 
' ┌──────────────────────────────────────────────────────────────┐
' │            Sets a single AY register to a value              │
' ├──────────────────────────────────────────────────────────────┤
' │ reg - The AY register to set.                                │
' │                                                              │
' │ val - The value to set the register to.                      │
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + reg] := val
  
PUB updateRegisters(source)
' ┌──────────────────────────────────────────────────────────────┐
' │                  Update all 16 AY registers                  │
' ├──────────────────────────────────────────────────────────────┤
' │ source - A pointer to an array containing 16 bytes to update │
' │          the 16 AY registers with.                           │
' └──────────────────────────────────────────────────────────────┘
  bytemove(@AYregisters, source, 13)
  ifnot byte[source + 13] == 255
    byte[@AYregisters + 13] := byte[source + 13]&15
 
PUB resetRegisters
' ┌──────────────────────────────────────────────────────────────┐
' │                  Reset all 16 AY registers                   │
' └──────────────────────────────────────────────────────────────┘
  bytefill(@AYregisters, 0, 16)

PUB setVolume(channel, volumeLevel)
' ┌──────────────────────────────────────────────────────────────┐
' │             Sets the volume of an AY channel                 │
' ├──────────────────────────────────────────────────────────────┤
' │ channel - The AY channel to set. (0 - 2)                     │
' │                                                              │
' │ volumeLevel - A value betwen 0 and 15.                       │
' │ (A volumeLevel of 16 enables the envelope on the channel)    │ 
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + 8 + channel] := volumeLevel
  
PUB setFreq(channel, freq)
' ┌──────────────────────────────────────────────────────────────┐
' │            Sets the frequency of an AY channel               │
' ├──────────────────────────────────────────────────────────────┤
' │ channel - The AY channel to set. (0 - 2)                     │
' │                                                              │ 
' │ freq - The 12 bit frequency value. (0 - 4095)                │
' ├──────────────────────────────────────────────────────────────┤ 
' │ Frequency in Hz = PSG_FREQ / 64 / freq                       │
' └──────────────────────────────────────────────────────────────┘
  word[@AYregisters + (channel<<1)] := freq
  
PUB setNoiseFreq(freq)
' ┌──────────────────────────────────────────────────────────────┐
' │        Sets the frequency of the noise generator             │
' ├──────────────────────────────────────────────────────────────┤
' │ freq - The 5 bit frequency value. (0 - 31)                   │
' ├──────────────────────────────────────────────────────────────┤ 
' │ Frequency in Hz = PSG_FREQ / 64 / freq                       │ 
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + 6] := freq

PUB enableTone(ch1, ch2, ch3)
' ┌──────────────────────────────────────────────────────────────┐
' │             Enable/Disable tone on AY channels               │
' ├──────────────────────────────────────────────────────────────┤
' │ ch1 - Enable/Disable tone on ch 1. (True/False)              │
' │                                                              │
' │ ch2 - Enable/Disable tone on ch 2. (True/False)              │  
' │                                                              │
' │ ch3 - Enable/Disable tone on ch 3. (True/False)              │   
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + 7] := (byte[@AYregisters + 7]&$F8) | (!ch1&1) | (!ch2&2) | (!ch3&4) 
      
PUB enableNoise(ch1, ch2, ch3)
' ┌──────────────────────────────────────────────────────────────┐
' │              Enable/Disable noise on channels                │
' ├──────────────────────────────────────────────────────────────┤
' │ ch1 - Enable/Disable noise on ch 1. (True/False)             │
' │                                                              │
' │ ch2 - Enable/Disable noise on ch 2. (True/False)             │  
' │                                                              │
' │ ch3 - Enable/Disable noisen on ch 3. (True/False)            │   
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + 7] := (byte[@AYregisters + 7]&$C7) | (!ch1&8) | (!ch2&16) | (!ch3&32) 

PUB setEnvelope(shape, rate)
' ┌──────────────────────────────────────────────────────────────┐
' │        Sets the properties of the envelope generator         │
' ├──────────────────────────────────────────────────────────────┤
' │ shape - The 4 bit shape value. (See table below)             │
' │                                                              │ 
' │ rate - The 16 bit envelope rate value. (0 - 65535)           │
' ├──────────────────────────────────────────────────────────────┤
' │                                                              │
' │  Bits     Shape     Description               Hold           │
' │ ───────   ─────     ───────────               ────           │
' │                                                              │
' │ 0 0 x x   \___      High to low               Low            │
' │                                                              │
' │ 0 1 x x   /___      Low to high               Low            │
' │                                                              │
' │ 1 0 0 0   \\\\      High to low ...           Loop           │
' │                                                              │
' │ 1 0 0 1   \___      High to low               Low            │
' │                                                              │
' │ 1 0 1 0   \/\/      High to low to high ...   Loop           │
' │                                                              │
' │ 1 0 1 1   \"""      High to low               High           │
' │                                                              │
' │ 1 1 0 0   ////      Low to high ...           Loop           │
' │                                                              │
' │ 1 1 0 1   /"""      Low to high               High           │

' │                                                              │
' │ 1 1 1 0   /\/\      Low to high to low  ...   Loop           │
' │                                                              │
' │ 1 1 1 1   /___      Low to high               Low            │
' │                                                              │ 
' ├──────────────────────────────────────────────────────────────┤
' │ Envelope step frequency = PSG_FREQ / 64 / rate               │
' │ (The envelope generator has 16 steps)                        │
' ├──────────────────────────────────────────────────────────────┤  
' │ NOTE!  NOTE!  NOTE!  NOTE!  NOTE!  NOTE!  NOTE!  NOTE!  NOTE!│
' │ To enable the envelope on an AY channel, set volume to 16    │  
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + 13] := shape  
  byte[@AYregisters + 11] := (rate&$ff) 
  byte[@AYregisters + 12] := (rate>>8)

PUB triggerEnvelope
' ┌──────────────────────────────────────────────────────────────┐
' │              Trigger the envelope generator                  │
' └──────────────────────────────────────────────────────────────┘
  byte[@AYregisters + 13] := byte[@AYregisters + 13]&15
    
dat org 0
'
'                Assembly AY emulator
'
AYEMU         mov      AY_Address, par                      
              mov      dira, r1
              mov      ctra, arg1
              mov      ctrb, arg2
              mov      waitCounter, cnt
              add      waitCounter, sampleRate

'
' Read all AY registers from hub memory and convert
' them to more convenient representations.
'
mainLoop 
getRegisters  mov       tempValue, AY_Address
              rdword    frequency1, tempValue               ' reg 0+1
              shl       frequency1, #20                   
              add       tempValue,  #2                      
              rdword    frequency2, tempValue               ' reg 2+3
              shl       frequency2, #20
              add       tempValue,  #2
              rdword    frequency3, tempValue               ' reg 4+5
              shl       frequency3, #20
              add       tempValue,  #2
              rdbyte    noisePeriod, tempValue              ' reg 6
              and       noisePeriod, #$1f
              add       tempValue, #1
              rdbyte    enableRegister, tempValue           ' reg 7
              min       noisePeriod, #2
              add       tempValue, #1
              rdbyte    amplitude1, tempValue               ' reg 8
              and       amplitude1, #31
              add       tempValue, #1
              rdbyte    amplitude2, tempValue               ' reg 9
              and       amplitude2, #31
              add       tempValue, #1
              rdbyte    amplitude3, tempValue               ' reg 10
              and       amplitude3, #31
              add       tempValue, #1
              rdbyte    envelopePeriod, tempValue           ' reg 11
              add       tempValue, #1
              shl       noisePeriod, #20
              rdbyte    temp1, tempValue                    ' reg 12
              shl       temp1, #8
              or        envelopePeriod, temp1           wz
        if_z  mov       envelopePeriod, half_period         ' 0 == half the period of 1
        if_nz shl       envelopePeriod, #16
              add       tempValue, #1
              rdbyte    envelopeShape, tempValue            ' reg 13
              movd      oscValues, enableRegister

'
' Calculate AY samples channel 1-3 and store in out1-out3
'

'───────────────────────────────────────────────────────────
'        Envelope shaping -> envelopeAmplitude
'───────────────────────────────────────────────────────────
Envelope      sub      envCounter, envSubValue           wc ' Handle envelope incrementation
  if_c        add      envCounter, envelopePeriod
  if_c        add      envelopeValue, envelopeInc
              max      envCounter, envelopePeriod
'───────────────────────────────────────────────────────────
              test     envelopeShape, #16                wz ' Handle envelope "reset bit" ( Extra bit added by Ahle2 )
  if_z        neg      envelopeValue, #0
  if_z        mov      envelopeInc, #1
  if_z        mov      envCounter, envelopePeriod
  if_z        or       envelopeShape, #16
  if_z        wrbyte   envelopeShape, tempValue             '<-IMPORTANT, sets bit 5 ("reset bit") of envelope shape register in hub ram
'───────────────────────────────────────────────────────────
              test     envelopeShape, #8                 wc ' Handle envelope continue = 0
              test     envelopeShape, #4                 wz
 if_nc_and_z  mov      envelopeShape, #9
 if_nc_and_nz mov      envelopeShape, #15
'───────────────────────────────────────────────────────────
              test     envelopeShape, #2                 wz ' Sets the envelope hold level
              muxz     envHoldLevel, #15                    '
'───────────────────────────────────────────────────────────
              test     envelopeValue, #16                wz ' Check if > 15
              test     envelopeShape, #1                 wc ' Check hold bit
  if_nz_and_c mov      envelopeInc, #0                      ' Hold envelope
  if_nz_and_c mov      envelopeValue, envHoldLevel          '
'───────────────────────────────────────────────────────────
  if_nz       test     envelopeShape, #2                 wc ' Check and handle envelope alternation
  if_nz_and_c neg      envelopeInc, envelopeInc
  if_nz_and_c add      envelopeValue, envelopeInc
'───────────────────────────────────────────────────────────
              mov      envelopeAmplitude, envelopeValue
              test     envelopeShape, #4                 wc ' Check and handle envelope invertion (attack)
  if_nc       xor      envelopeAmplitude, #15               '(Move Value or ~Value to envelopeAmplitude)
 
'───────────────────────────────────────────────────────────
'     Waveform shaping noise -> bit 3 of oscValues
'───────────────────────────────────────────────────────────
Noise1        sub      phaseAccumulatorN, noiseSubValue  wc ' Noise generator
  if_nc       jmp      #Env1
              add      phaseAccumulatorN, noisePeriod 
              test     noiseValue, noiseTap              wc
              muxc     noiseValue, noiseMSB
              shr      noiseValue, #1                    wc                                         
  if_c        xor      oscValues, #8                     

'───────────────────────────────────────────────────────────
'            Waveform shaping channel 1 -> out1
'───────────────────────────────────────────────────────────
Env1          test     amplitude1, #16                   wz ' Selects envelope or fixed amplitude
  if_nz       mov      amplitude1, envelopeAmplitude        ' depending on bit 5 of amplitude register 1
              mov      arg1, amplitude1
              call     #getAmplitude
'───────────────────────────────────────────────────────────
Square1       cmp      frequency1, freqRef               wc     
  if_nc       sub      phaseAccumulator1, oscSubValue    wc ' Square wave generator
  if_c        add      phaseAccumulator1, frequency1        ' channel 1
  if_c        xor      oscValues, #1
'───────────────────────────────────────────────────────────
              test     oscValues, mask513                wz ' Handles mixing of channel 1
              negnz    out1, r1                             ' Tone on/off, Noise on/off     
              test     oscValues, mask4104               wz
  if_z        mov      out1, r1                             ' out1 = (ToneOn | ToneDisable) & (NoiseOn | NoiseDisable)
 

'─────────────────────────────────────────────────────────── 
'            Waveform shaping channel 2 -> out2
'───────────────────────────────────────────────────────────
Env2          test     amplitude2, #16                   wz ' Selects envelope or fixed amplitude
  if_nz       mov      amplitude2, envelopeAmplitude        ' depending on bit 5 of amplitude register 2
              mov      arg1, amplitude2
              call     #getAmplitude
'───────────────────────────────────────────────────────────
Square2       cmp      frequency2, freqRef               wc        
  if_nc       sub      phaseAccumulator2, oscSubValue    wc ' Square wave generator
  if_c        add      phaseAccumulator2, frequency2        ' channel 2
  if_c        xor      oscValues, #2
'───────────────────────────────────────────────────────────
              test     oscValues, mask1026               wz ' Handles mixing of channel 2
              negnz    out2, r1                             ' Tone on/off, Noise on/off     
              test     oscValues, mask8200               wz
  if_z        mov      out2, r1                             ' out2 = (ToneOn | ToneDisable) & (NoiseOn | NoiseDisable)

'───────────────────────────────────────────────────────────              
'            Waveform shaping channel 3 -> out3
'───────────────────────────────────────────────────────────
Env3          test     amplitude3, #16                   wz ' Selects envelope or fixed amplitude
  if_nz       mov      amplitude3, envelopeAmplitude        ' depending on bit 5 of amplitude register 3
              mov      arg1, amplitude3
              call     #getAmplitude
'───────────────────────────────────────────────────────────
Square3       cmp      frequency3, freqRef               wc        
  if_nc       sub      phaseAccumulator3, oscSubValue    wc ' Square wave generator
  if_c        add      phaseAccumulator3, frequency3        ' channel 3
  if_c        xor      oscValues, #4
'───────────────────────────────────────────────────────────
              test     oscValues, mask2052               wz ' Handles mixing of channel 2
              negnz    out3, r1                             ' Tone on/off, Noise on/off 
              test     oscValues, mask16392              wz
  if_z        mov      out3, r1                             ' out3 = (ToneOn | ToneDisable) & (NoiseOn | NoiseDisable)

' 
'      Mix channels and update FRQA/FRQB PWM-values
'
mixer         mov      r1, val31bit                        '  DC offset
              add      r1, out1
              add      r1, out2
              add      r1, out3
              waitcnt  waitCounter, sampleRate             '  Wait until the right time to update
              mov      FRQA, r1                            '  the PWM values in FRQA/FRQB
              mov      FRQB, r1                            ' 
              jmp      #mainLoop 

' 
'    Get amplitude value  r1 = amplitudTable[arg1] 
'
getAmplitude  and      arg1, #15
              add      arg1, #amplitudeTable             
              movs     :indexed1, arg1                  
              nop
:indexed1     mov      r1, 0
getAmplitude_ret ret


' 
'    Variables, tables, masks and reference values
'
amplitudeTable      long trunc(AMPLITUDE_LEVEL_0) 
                    long trunc(AMPLITUDE_LEVEL_1) 
                    long trunc(AMPLITUDE_LEVEL_2)                     
                    long trunc(AMPLITUDE_LEVEL_3)                     
                    long trunc(AMPLITUDE_LEVEL_4)                       
                    long trunc(AMPLITUDE_LEVEL_5)                       
                    long trunc(AMPLITUDE_LEVEL_6)                      
                    long trunc(AMPLITUDE_LEVEL_7)                     
                    long trunc(AMPLITUDE_LEVEL_8)                     
                    long trunc(AMPLITUDE_LEVEL_9)                       
                    long trunc(AMPLITUDE_LEVEL_A) 
                    long trunc(AMPLITUDE_LEVEL_B)                      
                    long trunc(AMPLITUDE_LEVEL_C)                     
                    long trunc(AMPLITUDE_LEVEL_D)  
                    long trunc(AMPLITUDE_LEVEL_E) 
                    long trunc(AMPLITUDE_LEVEL_F)
                                                                                                                                                                                                                                                                                           
'Masks and reference values
mask513             long 513
mask1026            long 1026
mask2052            long 2052
mask4104            long 4104
mask8200            long 8200
mask16392           long 16392
mask32bit           long $ffffffff
mask16bit           long $ffff
half_period         long $00008000
val31bit            long $80000000
noiseMSB            long 1 << NOISE_MSB
noiseTap            long NOISE_TAP
sampleRate          long 0
freqRef             long 10<<20

'Setup and subroutine parameters  
arg1                long 0
arg2                long 0
r1                  long 0
AY_Address          long 0

'AY variables
noiseValue          long $ffff 
envCounter          long 1
envSubValue         long ENV_CORR
oscSubValue         long OSC_CORR
noiseSubValue       long NOISE_CORR
envelopeValue       long 0
envelopeInc         long 1
envHoldLevel        res  1
oscValues           res  1
amplitude1          res  1
amplitude2          res  1
amplitude3          res  1
envelopeAmplitude   res  1
enableRegister      res  1
envelopeShape       res  1
frequency1          res  1
frequency2          res  1
frequency3          res  1
envelopePeriod      res  1
noisePeriod         res  1
phaseAccumulatorN   res  1
phaseAccumulator1   res  1
phaseAccumulator2   res  1
phaseAccumulator3   res  1
noiseOut            res  1
out1                res  1
out2                res  1
out3                res  1
waitCounter         res  1
tempValue           res  1
temp1               res  1

                    fit  $1F0

VAR
  long cog
  
  long AYregisters[4]  '(= 16 AY registers) 
' Reg bits function
' -----------------------------------
' 00  7..0 channel A fine tune
' 01  3..0 channel A coarse tune
' 02  7..0 channel B fine tune
' 03  3..0 channel B coarse tune
' 04  7..0 channel C fine tune
' 05  3..0 channel C coarse tune
' 06  4..0 noise period
' 07  7..0 enable register
' 08  4..0 channel A volume
' 09  4..0 channel B volume
' 10  4..0 channel C volume
' 11  7..0 envelope fine tune
' 12  7..0 envelope coarse tune
' 13  3..0 envelope shape
' 14  7..0 I/O port A value
' 15  7..0 I/O port B value 
  
