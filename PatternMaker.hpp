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
  static GroupSong* MakeAugmented(ISonglet& song, int Key) {
    Key /= SemitoneFraction;// convert from octaves to frets
    return Create_Triad(song, Key, (Key + 4), (Key + 8));
  }
  /* ********************************************************************************* */
  static GroupSong* MakeDiminished(ISonglet& song, int Key) {
    Key /= SemitoneFraction;// convert from octaves to frets
    return Create_Triad(song, Key, (Key + 3), (Key + 6));
  }
  /* ********************************************************************************* */
  static GroupSong* Create_Seventh(ISonglet& song, int NoteDex0, int NoteDex1, int NoteDex2, int NoteDex3) {
    double Loudness = 1.0;// NoteDex0 is usually the key
    double Duration = 2.0;
    GroupSong *cbx = Create_Triad(song, NoteDex0, NoteDex1, NoteDex2);
    cbx->Add_SubSong(song, 0, SemitoneFraction * NoteDex3, Loudness);
    return cbx;
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
  /* ********************************************************************************* */
  static void Create_Tapered_Voice(Voice& voice, double TimeOffset, double Duration, double OctaveOffset, double LoudnessOffset, int numsteps) {
    double AttackTime = 0.01;
    Duration -= AttackTime;
    double midfrac;
    voice.Add_Note(TimeOffset, OctaveOffset, 0);
    for (int cnt = 0; cnt <= numsteps; cnt++) {
      midfrac = ((double) cnt) / (double) numsteps;
      voice.Add_Note(TimeOffset + AttackTime + (Duration * midfrac), OctaveOffset, LoudnessOffset * (1.0 - midfrac));
    }
  }
  /* ********************************************************************************* */
  static OffsetBoxBase* Recurse(ArrayList<ISonglet*>& palette, int Depth){// Attempted random song maker, under construction.
    ISonglet *child=nullptr, *grandkid=nullptr;//  does this really not make circular references?
    Voice *voz; GroupSong *gsong; LoopSong *lsong;
    OffsetBoxBase *GrandkidHandle;
    int randex;
    double MajorMinorOdds = 0.5;
    double MajorMinorKey = 0.0;
    double NumWays = 0;
    double VoiceThresh = (NumWays += 0.1);// Voice
    double PaletteThresh = (NumWays += palette.size());
    double LoopThresh = (NumWays += 1.0);// Loop
    double GroupThresh = (NumWays += 1.0);// Group
    double MajorThresh = (NumWays += MajorMinorOdds);// Major chord
    double MinorThresh = (NumWays += MajorMinorOdds);// Minor chord

    PaletteThresh /= NumWays; VoiceThresh /= NumWays; LoopThresh /= NumWays;
    GroupThresh /= NumWays; MajorThresh /= NumWays; MinorThresh /= NumWays;

    Depth++;

    double dice = Math::frand();
    if (dice<VoiceThresh || Depth>3){// end with a leaf
      printf("Voice\n");
      child = voz = new Voice();
      double Duration=0.25, OctaveOffset=0.0, LoudnessOffset=1.0; int numsteps = 3;
      Duration = (Math::frand() * 0.8) + 0.2;
      Create_Tapered_Voice(*voz, 0.0, Duration, OctaveOffset, LoudnessOffset, numsteps);// fill in voice
    } else if (dice<PaletteThresh){
      printf("Palette\n");
      randex = std::rand() % palette.size();
      child = palette[randex];
    } else if (dice<LoopThresh){
      printf("Loop\n");
      child = lsong = new LoopSong();
      GrandkidHandle = Recurse(palette, Depth);
      lsong->Add_SubSong(GrandkidHandle);
      grandkid = GrandkidHandle->GetContent();
      if (!palette.Contains(grandkid)){
        palette.Add(grandkid);
      }
      // Set interval and beats
      SoundFloat random = (Math::frand()*1.5) + 0.25;// 0.123456;//
      lsong->Set_Interval(random);
      randex = (std::rand() % 12)+2;
      lsong->Set_Beats(randex);
    } else if (dice<GroupThresh){
      printf("Group\n");
      child = gsong = new GroupSong();
      randex = (std::rand() % 12)+2;// arbitrary max members of a group
      for (int cnt=0; cnt<randex; cnt++){
        GrandkidHandle = Recurse(palette, Depth);
        gsong->Add_SubSong(GrandkidHandle);
        grandkid = GrandkidHandle->GetContent();
        if (!palette.Contains(grandkid)){
          palette.Add(grandkid);
        }
      }
    } else if (dice<MajorThresh){
      printf("Major\n");
      GrandkidHandle = Recurse(palette, Depth);
      grandkid = GrandkidHandle->GetContent();
      child = gsong = MakeMajor(*grandkid, MajorMinorKey);
      delete GrandkidHandle;
      if (!palette.Contains(grandkid)){
        palette.Add(grandkid);
      }
    } else if (dice<MinorThresh){
      printf("Minor\n");
      GrandkidHandle = Recurse(palette, Depth);
      grandkid = GrandkidHandle->GetContent();
      child = gsong = MakeMinor(*grandkid, MajorMinorKey);
      delete GrandkidHandle;
      if (!palette.Contains(grandkid)){
        palette.Add(grandkid);
      }
    }

    SoundFloat random_time = (Math::frand()*1.5) + 0.2;
    SoundFloat random_octave = (Math::frand()*2.0) + 0.1;
    SoundFloat random_loudness=1.0;
    OffsetBoxBase *obox = child->Spawn_OffsetBox();
    obox->TimeX = random_time;
    obox->OctaveY = random_octave;
    obox->LoudnessFactor = random_loudness;
    return obox;
  }
  /* ********************************************************************************* */
  static LoopSong* MakeRandom(){// under construction
    ArrayList<ISonglet*> palette; // things that we've created so far

    OffsetBoxBase *obox = Recurse(palette, 0);
    obox->OctaveY = 4.0;

    if (true) {
      Config conf;
      MetricsPacket metrics;
      metrics.MaxDuration = 0.0;
      metrics.MyProject = &conf;
      metrics.FreshnessTimeStamp = 1;
      ISonglet *songlet = obox->GetContent();
      printf("MakeRandom Update_Guts:\n");
      songlet->Update_Guts(metrics);
      printf("MakeRandom Update_Guts done.\n");
    }

    LoopSong *MainLoop = new LoopSong();
    MainLoop->Add_SubSong(obox);

    //Voice voz;
    //MainLoop->Add_SubSong(voz, (SoundFloat)0.0, (SoundFloat)4.0, (SoundFloat)1.0);// this should work! inherits from GroupSong.
    //MainLoop.Add_SubSong(*songlet, (SoundFloat)0.0, (SoundFloat)4.0, (SoundFloat)1.0);// this should work! inherits from GroupSong.
    SoundFloat LoopInterval = obox->GetContent()->Get_Duration();// 0.123456;
    //LoopInterval = 2.0;
    MainLoop->Set_Interval(LoopInterval);
    MainLoop->Set_Beats(9);
    printf("LoopInterval:%f\n", LoopInterval);
    return MainLoop;
  }
};

#endif // PatternMaker_hpp
