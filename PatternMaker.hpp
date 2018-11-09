#ifndef PatternMaker_hpp
#define PatternMaker_hpp

#include "Voice.hpp"
#include "GroupSong.hpp"

class PatternMaker {
public:
  static constexpr double NumNotes = 12;
  static constexpr double SemitoneFraction = (1.0 / NumNotes);
  static constexpr double OffsetTime = 0.03;
  /* ********************************************************************************* */
  static GroupSong* MakeMajor(ISonglet& song, double Key) {
    Key /= SemitoneFraction;// convert from octaves to frets
    return Create_Triad(song, Key, (Key + 4), (Key + 7));
  }
  /* ********************************************************************************* */
  static GroupSong* MakeMinor(ISonglet& song, double Key) {
    Key /= SemitoneFraction;// convert from octaves to frets
    return Create_Triad(song, Key, (Key + 3), (Key + 7));
  }
  /* ********************************************************************************* */
  static GroupSong* Create_Triad(ISonglet& song, double NoteDex0, double NoteDex1, double NoteDex2) {
    double Loudness = 1.0;// NoteDex0 is usually the key
    GroupSong *gsong = new GroupSong();
    gsong->Add_SubSong(song, 0 + OffsetTime, SemitoneFraction * NoteDex0, Loudness);
    gsong->Add_SubSong(song, 0 + OffsetTime, SemitoneFraction * NoteDex1, Loudness);
    gsong->Add_SubSong(song, 0 + OffsetTime, SemitoneFraction * NoteDex2, Loudness);
    return gsong;
  }
  /* ********************************************************************************* */
  static Voice* Create_Bent_Note(double TimeOffset, double Duration, double OctaveOffset, double LoudnessOffset) {
    Voice *voice = new Voice();
    double midfrac0 = 0.03, midfrac1 = 0.5;
    voice->Add_Note(TimeOffset + 0, OctaveOffset + 0, LoudnessOffset * 0);
    voice->Add_Note(TimeOffset + Duration * midfrac0, OctaveOffset + 0, LoudnessOffset * 1);
    voice->Add_Note(TimeOffset + Duration * midfrac1, OctaveOffset - 0.07, LoudnessOffset * midfrac1);
    voice->Add_Note(TimeOffset + Duration, OctaveOffset + 0.0, LoudnessOffset * 0.0);
    //voice->Add_Note(TimeOffset + Duration, OctaveOffset + 0.1, LoudnessOffset * 0);
    return voice;
  }
  /* ********************************************************************************* */
  static LoopSong* Create_Unbound_Triad_Rhythm(ISonglet& song) {
    OffsetBoxBase *obox = nullptr;
    GroupSong *CMinor = nullptr, *CMajor = nullptr, *DMajor = nullptr, *DMinor = nullptr;
    double Delay = 1.5;
    double Key = 0.0;//5;//0.0;// relative key, no offset from master group
    GroupSong *gsong = nullptr;
    LoopSong *lsong = nullptr;

    double offx = OffsetTime;
    gsong = new GroupSong();
    CMajor = MakeMajor(song, Key);// C major
    gsong->Add_SubSong(*CMajor, 0 + offx, 0, 1.0);

    CMinor = MakeMinor(song, Key);// C minor
    gsong->Add_SubSong(*CMinor, Delay * 1 + offx, 0, 1.0);

    DMajor = MakeMajor(song, Key);// D major
    gsong->Add_SubSong(*DMajor, Delay * 2 + offx, 2.0 * SemitoneFraction, 1.0);

    DMinor = MakeMinor(song, Key);// D minor
    gsong->Add_SubSong(*DMinor, Delay * 3 + offx, 2.0 * SemitoneFraction, 1.0);

    OffsetBoxBase *gobx = gsong->Spawn_OffsetBox();
    gobx->OctaveY = 5;

    lsong = new LoopSong();
    lsong->Add_SubSong(gobx);

    lsong->Set_Interval(Delay * 4.0);// 4 * 1.5 seconds per cycle // lsong->Set_Delay(Delay * 4.0);
    lsong->Set_Beats(10);// lsong->Set_Duration(30.0);

    return lsong;
  }
};

#endif // PatternMaker_hpp
