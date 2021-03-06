#ifndef GroupSong_hpp
#define GroupSong_hpp

#include <iostream>
#include <sstream>  // Required for stringstreams
#include <string>
#include <vector>
#include "IDrawable.hpp"
#include "ISonglet.hpp"

/**
 *
 * @author MultiTool
*/

// class AudProject;// forward

class GroupSong: public ISonglet {
public:
  ArrayList<OffsetBoxBase*> SubSongs;
  String SubSongsName = "SubSongs";
  SoundFloat Duration = 0.0;
  String MyName;// for debugging
  SoundFloat MaxAmplitude = 0.0;
  CajaDelimitadora MyBounds;

  // to do: These will belong to the Artist object after we code for that separation
  boolean HighlightSpine = false;
  Color* LineColor;
  boolean UsingSplines = true;
  int NumSubLines = 10;
  ArrayList<Point2D> SplinePoints;
  /* ********************************************************************************* */
  GroupSong() {
    RefCount = 0;
    //this->LineColor = Globals::ToColorWheel(Math::frand());
    FreshnessTimeStamp = 0;
  }
  ~GroupSong() {this->Delete_Me();}
  /* ********************************************************************************* */
  virtual OffsetBoxBase* Add_SubSong(ISonglet& songlet, SoundFloat TimeOffset, SoundFloat OctaveOffset, SoundFloat LoudnessFactor) {
    OffsetBoxBase *obox = songlet.Spawn_OffsetBox();
    this->Add_SubSong(obox, TimeOffset, OctaveOffset, LoudnessFactor);
    return obox;// even though we return the offsetbox, it is groupsong's responsibility to delete the offsetbox.
  }
  /* ********************************************************************************* */
  void Add_SubSong(OffsetBoxBase* obox, SoundFloat TimeOffset, SoundFloat OctaveOffset, SoundFloat LoudnessFactor) {// Add a songlet with its offsetbox already created.
    obox->TimeX = (TimeOffset);
    obox->OctaveY = (OctaveOffset);
    obox->LoudnessFactor = (LoudnessFactor);
    this->Add_SubSong(obox);
  }
  /* ********************************************************************************* */
  virtual int Add_SubSong(OffsetBoxBase* obox) {// Add a songlet with its offsetbox already created and filled out.
    obox->GetContent()->Set_Project(this->MyProject);// child inherits project from me
    obox->MyParentSong = this;
    int dex = this->Tree_Search(obox->TimeX, 0, SubSongs.size());
    SubSongs.insert(SubSongs.begin() + dex, obox);
    int sz = SubSongs.size();
    //this->Sort_Me();// overkill in the case we add a bunch of subsongs in a loop. should only sort once at end of loop.
    Refresh_Splines();// maybe sort_me and refresh_splines should be in update_guts instead?
    return dex;
  }
  /* ********************************************************************************* */
  void Remove_SubSong(OffsetBoxBase& obox) {
    this->SubSongs.remove(&obox);
//    ISonglet *song = obox.GetContent();// we should leave this cleanup to whomever called us.
//    int RefCnt = song->UnRef_Songlet();
//    if (RefCnt<=0){ delete song; }
    Refresh_Splines();
  }
  /* ********************************************************************************* */
  void Refresh_Splines() {
    int SplineSize = (this->SubSongs.size()) * NumSubLines + 1;
    this->SplinePoints.resize(SplineSize);
    Splines::Cubic_Spline_Boxes(this->SubSongs, NumSubLines, SplinePoints);
  }
  /* ********************************************************************************* */
  SoundFloat Get_Duration() override {
    return this->Duration;
  }
  /* ********************************************************************************* */
  SoundFloat Get_Max_Amplitude() override {
    return this->MaxAmplitude;
  }
  /* ********************************************************************************* */
  void Update_Max_Amplitude() {
    int len = this->SubSongs.size();
    OffsetBoxBase *pnt;
    SoundFloat MaxAmp = 0.0;
    for (int pcnt = 0; pcnt < len; pcnt++) {
      pnt = this->SubSongs.at(pcnt);
      SoundFloat Amp = pnt->Get_Max_Amplitude();
      MaxAmp += Amp;// this is overkill, we need to only sum those subsongs that overlap.
    }
    this->MaxAmplitude = MaxAmp;
  }
  /* ********************************************************************************* */
//  SoundFloat Update_Durations() override {
//    SoundFloat MaxDuration = 0.0;
//    SoundFloat DurBuf = 0.0;
//    int NumSubSongs = this->SubSongs.size();
//    for (int cnt = 0; cnt < NumSubSongs; cnt++) {
//      OffsetBoxBase *obox = this->SubSongs.at(cnt);
//      ISonglet *child = obox->GetContent();
//      //if (MaxDuration < (DurBuf = (ob.UnMapTime(vb.Update_Durations())))) {
//      if (MaxDuration < (DurBuf = (obox->TimeX + child->Update_Durations()))) {
//        MaxDuration = DurBuf;
//      }
//    }
//    this->Duration = MaxDuration;
//    return MaxDuration;
//  }
  /* ********************************************************************************* */
  void Update_Guts(MetricsPacket& metrics) override {
    if (this->FreshnessTimeStamp != metrics.FreshnessTimeStamp) {// don't hit the same songlet twice on one update
      this->Set_Project(metrics.MyProject);// should probably separate Set_Project into its own thing
      //this->Sort_Me();
      this->Update_Max_Amplitude();
      metrics.MaxDuration = 0.0;// redundant
      SoundFloat MyMaxDuration = 0.0;
      SoundFloat DurBuf = 0.0;
      int NumSubSongs = this->SubSongs.size();
      for (int cnt = 0; cnt < NumSubSongs; cnt++) {
        OffsetBoxBase *obx = this->SubSongs.at(cnt);
        ISonglet *songlet = obx->GetContent();
        metrics.MaxDuration = 0.0;
        songlet->Update_Guts(metrics);
        if (MyMaxDuration < (DurBuf = (obx->TimeX + metrics.MaxDuration))) {
          MyMaxDuration = DurBuf;
        }
      }
      this->Duration = MyMaxDuration;
      this->FreshnessTimeStamp = metrics.FreshnessTimeStamp;
    }
    metrics.MaxDuration = this->Duration;
  }
  /* ********************************************************************************* */
  void Refresh_Me_From_Beneath(IMoveable& mbox) override {
    //System.out.println("Refresh_Me_From_Beneath");
    this->Sort_Me();
//    int Dex = this->Tree_Search(mbox.GetX(), 0, this->SubSongs.size());
//    this->Bubble_Right(Dex);// how do we get the right index? OffsetBoxes do not have that.
    if (UsingSplines) {
      Splines::Cubic_Spline_Boxes(this->SubSongs, NumSubLines, this->SplinePoints);
    }
  }
  /* ********************************************************************************* */
  void Bubble_Right(int Dex) {// when a point moves right in space, move it to the correct place in the collection.
    int len = this->SubSongs.size();
    OffsetBoxBase *mov = this->SubSongs.at(Dex);
    OffsetBoxBase *next;
    int PrevDex = Dex++;
    while (Dex < len) {
      next = this->SubSongs.at(Dex);
      if (mov->TimeX <= next->TimeX) {
        break;
      }
      this->SubSongs.at(PrevDex) = next;
      PrevDex = Dex++;
    }
    this->SubSongs.at(PrevDex) = mov;
  }
  /* ********************************************************************************* */
  void Sort_Me() {// override {// sorting by RealTime, TimeX
    std::sort(this->SubSongs.begin(), this->SubSongs.end(), CompareChildren);
  }
  /* ********************************************************************** */
  static bool CompareChildren(const OffsetBoxBase* obox0, const OffsetBoxBase* obox1) {
    return obox0->TimeX < obox1->TimeX;
  }
  /* ********************************************************************************* */
  void Set_Project(Config* project) override {
    this->MyProject = project;
    // below here may be redundant with Update_Guts
    int NumSubSongs = this->SubSongs.size();
    for (int cnt = 0; cnt < NumSubSongs; cnt++) {
      OffsetBoxBase *obx = this->SubSongs.at(cnt);
      ISonglet *songlet = obx->GetContent();
      songlet->Set_Project(this->MyProject);
    }
  }
  /* ********************************************************************************* */
  void SetSpineHighlight(boolean Highlight) {
    this->HighlightSpine = Highlight;
  }
  /* ********************************************************************************* */
  void Draw_Lines(IDrawingContext& ParentDC, int StartDex, int EndDex) {}
  /* ********************************************************************************* */
  void Draw_Me(IDrawingContext& ParentDC) override {// IDrawable
  }
  CajaDelimitadora* GetBoundingBox() override {// IDrawable
    return &(this->MyBounds);
  }
  void UpdateBoundingBox() override {// IDrawable
    OffsetBoxBase *ChildOffsetBox;
//    CajaDelimitadora ChildBBoxUnMapped;
//    this->MyBounds.Reset();
    int len = this->SubSongs.size();
    for (int pcnt = 0; pcnt < len; pcnt++) {
      ChildOffsetBox = this->SubSongs.at(pcnt);
      ChildOffsetBox->UpdateBoundingBox();
//      ChildBBoxUnMapped = ChildOffsetBox.GetBoundingBox();// project child limits into parent (my) space
//      this->MyBounds.Include(ChildBBoxUnMapped);
    }
    this->UpdateBoundingBoxLocal();
  }
  void UpdateBoundingBoxLocal() override {// IDrawable
    OffsetBoxBase *ChildOffsetBox;
    CajaDelimitadora *ChildBBoxUnMapped;
    this->MyBounds.Reset();
    int len = this->SubSongs.size();
    if (len == 0) {
      this->MyBounds.ClearZero();
    } else {
      for (int pcnt = 0; pcnt < len; pcnt++) {
        ChildOffsetBox = this->SubSongs.at(pcnt);
        ChildBBoxUnMapped = ChildOffsetBox->GetBoundingBox();// project child limits into parent (my) space
        this->MyBounds.Include(*ChildBBoxUnMapped);
      }
    }
  }
  /* ********************************************************************************* */
  void GoFishing(IGrabber& Scoop) override {// IDrawable
    if (Scoop.Intersects(MyBounds)) {// current search bounds are in parent coords
      int len = this->SubSongs.size();
      OffsetBoxBase *child;
      for (int pcnt = 0; pcnt < len; pcnt++) {
        child = this->SubSongs.at(pcnt);
        child->GoFishing(Scoop);
      }
    }
  }
  /* ********************************************************************************* */
  GroupSong* Clone_Me() override {// ICloneable
    GroupSong *child = new GroupSong();// clone
    child->Copy_From(*this);
    return child;
  }
  /* ********************************************************************************* */
  GroupSong* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
    GroupSong *child;
    CollisionItem *ci = HitTable.GetItem(this);
    if (ci == null) {
      child = new GroupSong();// clone
      ci = HitTable.InsertUniqueInstance(this);
      ci->Item = child;
      child->Copy_From(*this);
      OffsetBoxBase *SubSongHandle;
      int len = this->SubSongs.size();
      for (int cnt = 0; cnt < len; cnt++) {
        SubSongHandle = this->SubSongs.at(cnt);
        OffsetBoxBase *obox = SubSongHandle->Deep_Clone_Me(HitTable);
        child->Add_SubSong(obox);
      }
    } else {// pre exists
      child = (GroupSong*) ci->Item;// another cast!
    }
    return child;
  }
  /* ********************************************************************************* */
  GroupSong* Shallow_Clone_Me() {
    /*
     clone-me clones myself, clones all my children oboxes, but does NOT clone their songlets.
     */
    GroupSong *child;
    child = new GroupSong();// clone
    //child->TraceText = "I am a shallow clone";
    child->Copy_From(*this);
    OffsetBoxBase *SubSongHandle, *ChildSubSongHandle;
    ISonglet *songlet;
    int len = this->SubSongs.size();
    for (int cnt = 0; cnt < len; cnt++) {
      SubSongHandle = this->SubSongs.at(cnt);
      songlet = SubSongHandle->GetContent();
      ChildSubSongHandle = songlet->Spawn_OffsetBox();
      ChildSubSongHandle->Copy_From(*SubSongHandle);
      child->Add_SubSong(ChildSubSongHandle);
    }
    return child;
  }
  /* ********************************************************************************* */
  void Copy_From(const GroupSong& donor) {
    this->Duration = donor.Duration;
    this->Set_Project(donor.MyProject);
    this->MyName = donor.MyName;// for debugging
    this->MaxAmplitude = donor.MaxAmplitude;
    this->FreshnessTimeStamp = 0;// donor.FreshnessTimeStamp;
    this->MyBounds.Copy_From(donor.MyBounds);
  }
  /* ********************************************************************************* */
  void RescaleGroupTimeX(SoundFloat Factor) {
    int len = this->SubSongs.size();
    for (int cnt = 0; cnt < len; cnt++) {
      OffsetBoxBase *obox = this->SubSongs.at(cnt);
      obox->TimeX *= Factor;
    }
  }
  /* ********************************************************************************* */
  static SoundFloat DotProduct(SoundFloat X0, SoundFloat Y0, SoundFloat X1, SoundFloat Y1) {
    return X0 * X1 + Y0 * Y1;// length of projection from one vector onto another
  }
  /* ********************************************************************************* */
  void LineClosestPoint(SoundFloat LineX0, SoundFloat LineY0, SoundFloat LineX1, SoundFloat LineY1, SoundFloat XPnt, SoundFloat YPnt, Point2D& Intersection) {// Find dnd destination using dot product on line segments.
    SoundFloat Temp;
    if (LineX1 < LineX0 || (LineX1 == LineX0 && LineY1 < LineY0)) {// sort endpoints
      Temp = LineX0;// swap X
      LineX0 = LineX1;
      LineX1 = Temp;

      Temp = LineY0;// swap Y
      LineY0 = LineY1;
      LineY1 = Temp;
    }
    SoundFloat XDif = LineX1 - LineX0, YDif = LineY1 - LineY0;
    SoundFloat Shrink = Globals::Fudge;// shrink is a cheat so consecutive lines have slightly separate endpoints. this makes mouse distance from their endpoints unequal.
    SoundFloat ShrinkX = XDif * Shrink, ShrinkY = YDif * Shrink;
    LineX0 += ShrinkX;
    LineY0 += ShrinkY;
    LineX1 -= ShrinkX;
    LineY1 -= ShrinkY;
    XDif -= ShrinkX * 2;
    YDif -= ShrinkY * 2;
    SoundFloat Magnitude = Math::hypot(XDif, YDif);

    SoundFloat DotProd = DotProduct(XPnt - LineX0, YPnt - LineY0, XDif, YDif);
    DotProd /= Magnitude;// now dotprod is the full length of the projection
    SoundFloat XLoc = ((XDif / Magnitude) * DotProd);// scale separate dimensions to length of shadow
    SoundFloat YLoc = ((YDif / Magnitude) * DotProd);
    if (XLoc > XDif) {// Test if the intersection is between the line's endpoints and cap them.
      //System.out.println("XLoc:" + XLoc + ", YLoc:" + YLoc);
      XLoc = XDif;
      YLoc = YDif;
    } else if (XLoc < 0) {
      XLoc = YLoc = 0;
    }
    XLoc += LineX0 + ShrinkX;
    YLoc += LineY0 + ShrinkY;
    Intersection.SetLocation(XLoc, YLoc);
}
  /* ********************************************************************************* */
  static SoundFloat DistanceFromLine(SoundFloat LineX0, SoundFloat LineY0, SoundFloat LineX1, SoundFloat LineY1, SoundFloat XPnt, SoundFloat YPnt) {// work in progress for drag and drop support
    SoundFloat XDif = LineX1 - LineX0, YDif = LineY1 - LineY0;
    SoundFloat DotProd = DotProduct(XDif, YDif, XPnt - LineX0, YPnt - LineY0);
    SoundFloat XLoc = LineX0 + (XDif * DotProd);// point of intersection
    SoundFloat YLoc = LineY0 + (YDif * DotProd);
    // to do: at this point we would like to test if the intersection is between the line's endpoints.
//    SoundFloat XLoc = (XDif * DotProd);// point of intersection
//    SoundFloat YLoc = (YDif * DotProd);
//    SoundFloat PntDX = (XLoc - XPnt); SoundFloat PntDY = (YLoc - YPnt);
    SoundFloat Distance = Math::hypot(XLoc - XPnt, YLoc - YPnt);
    return Distance;
  }
  /* ********************************************************************************* */
  SoundFloat HeightFromLine(SoundFloat LineX0, SoundFloat LineY0, SoundFloat LineX1, SoundFloat LineY1, SoundFloat XPnt, SoundFloat YPnt) {// work in progress for drag and drop support
    SoundFloat YCross = LineYCross(LineX0, LineY0, LineX1, LineY1, XPnt);
    SoundFloat Distance = YPnt - YCross;
    return Distance;
  }
  /* ********************************************************************************* */
  SoundFloat LineYCross(SoundFloat LineX0, SoundFloat LineY0, SoundFloat LineX1, SoundFloat LineY1, SoundFloat XPnt) {// work in progress for drag and drop support
    SoundFloat XDif = LineX1 - LineX0;// given a line and an X point, return the Y location of the intersect along that line
    SoundFloat YCross;
    if (XDif == 0) {// If slope is infinite, just return the halfway point between the top and bottom of the line.
      YCross = (LineY0 + LineY1) / 2.0;
    } else {
      SoundFloat YDif = LineY1 - LineY0;
      SoundFloat Slope = YDif / XDif;
      SoundFloat PointXOffset = (XPnt - LineX0);
      YCross = LineY0 + (PointXOffset * Slope);
    }
    return YCross;
  }
  /* ********************************************************************************* */
  SoundFloat HitsMyVineSpline(SoundFloat XPnt, SoundFloat YPnt) {// work in progress for drag and drop support
    SoundFloat Limit = 0.1;// octaves.  hardcoded hack, need something better
    int len = this->SubSongs.size();
    SoundFloat Dist;
    OffsetBoxBase *LastBox = this->SubSongs.at(len - 1);
    Point2D Intersection;
    if (0.0 <= XPnt && XPnt <= LastBox->TimeX) {// or this->MyBounds.Max.x) {
      int FoundDex = Tree_Search(XPnt, 0, len);// to do: tree search with buffer around click point
      int LinesPerSubSong = this->SplinePoints.size() / this->SubSongs.size();

      // Splines start to the left of subsong[0], so their indexes are higher than the subsongs they correspond to.
      int StartSplineDex = FoundDex * LinesPerSubSong;
      int EndSplineDex = (FoundDex + 1) * LinesPerSubSong;
      EndSplineDex = Math::min(EndSplineDex, this->SplinePoints.size());

      Point2D prevpnt, pnt(0, 0);// = Point2D.Zero;
      SoundFloat MinDist = Double_POSITIVE_INFINITY;
      for (int scnt = StartSplineDex; scnt < EndSplineDex; scnt++) {// roll through all segments in this subrange of this spline. look for closest segment.
        prevpnt = pnt;
        pnt = this->SplinePoints[scnt];
        LineClosestPoint(prevpnt.x, prevpnt.y, pnt.x, pnt.y, XPnt, YPnt, Intersection);
        Dist = Math::hypot(XPnt - Intersection.x, YPnt - Intersection.y);
        //System.out.println("Dist:" + Dist + ", Intersection.x:" + Intersection.x + ", Intersection.y:" + Intersection.y);
        if (MinDist > Dist) {
          MinDist = Dist;
        }
      }
      if (MinDist < Limit) {// then we found one
        return MinDist;
      }
      // to do: need condition if FoundDex is greater than len. beyond-end insertion would be nice.
    }
    return Double_POSITIVE_INFINITY;// infinite if not found
  }
  /* ********************************************************************************* */
  SoundFloat HitsMyVine(SoundFloat XPnt, SoundFloat YPnt) {// work in progress for drag and drop support
    SoundFloat Limit = 0.1;// octaves.  hardcoded hack, need something better
    int len = this->SubSongs.size();
    OffsetBoxBase *OBox, *ClosestPoint = null;
    SoundFloat XPrev = 0, YPrev = 0, YCross, YDist, Dist;
    OffsetBoxBase *LastBox = this->SubSongs.at(len - 1);
    Point2D Intersection;
    if (0.0 <= XPnt && XPnt <= LastBox->TimeX) {// or this->MyBounds.Max.x) {
//      int FoundDex = Tree_Search(XPnt - Limit, 0, len);
      int FoundDex = Tree_Search(XPnt, 0, len);
      if (FoundDex == 0) {// X point equals first element in subsong array
        XPrev = YPrev = 0;
      } else {
        OffsetBoxBase *PrevBox = this->SubSongs.at(FoundDex - 1);
        XPrev = PrevBox->TimeX;
        YPrev = PrevBox->OctaveY;
      }
      // to do: need condition if FoundDex is greater than len. beyond-end insertion would be nice.
      OBox = this->SubSongs.at(FoundDex);

      if (true) {
        LineClosestPoint(XPrev, YPrev, OBox->TimeX, OBox->OctaveY, XPnt, YPnt, Intersection);
        Dist = Math::hypot(XPnt - Intersection.x, YPnt - Intersection.y);
        //System.out.println("Dist:" + Dist + ", Intersection.x:" + Intersection.x + ", Intersection.y:" + Intersection.y);
        if (Dist < Limit) {// then we found one
          ClosestPoint = this->SubSongs.at(FoundDex);
          return Dist;
        }
      } else {
        YCross = LineYCross(XPrev, YPrev, OBox->TimeX, OBox->OctaveY, XPnt);
        YDist = Math::abs(YPnt - YCross);
        if (YDist < Limit) {// then we found one
          ClosestPoint = this->SubSongs.at(FoundDex);
          return YDist;
        }
      }
    }
    return Double_POSITIVE_INFINITY;// infinite if not found
  }
  /* ************************************************************************************************************************ */
  int Tree_Search(SoundFloat Time, int minloc, int maxloc) {// finds place where time would be inserted or replaced
    int medloc;
    while (minloc < maxloc) {
      medloc = (minloc + maxloc) >> 1; // >>1 is same as div 2, only faster.
      if (Time <= this->SubSongs.at(medloc)->TimeX) {
        maxloc = medloc;
      } else {
        minloc = medloc + 1;/* has to go through here to be found. */
      }
    }
    return minloc;
  }
  /* ********************************************************************************* */
  void Space_Evenly() {
    int NumKids = this->SubSongs.size();
    if (NumKids == 0) {
      return;
    }
    int FinalKid = NumKids - 1;
    OffsetBoxBase *obox = this->SubSongs.at(FinalKid);
    SoundFloat XLimit = obox->TimeX;
    SoundFloat Spacing = XLimit / NumKids;
    SoundFloat FractAlong;
    for (int cnt = 0; cnt <= FinalKid; cnt++) {// do we want it to be  ---.---.---.  or  .---.---.  or  .---.---.---  ?
      obox = this->SubSongs.at(cnt);
      FractAlong = (((SoundFloat) cnt) / (SoundFloat) FinalKid);//  .---.---.  spacing
      obox->TimeX = XLimit * FractAlong;
    }
  }
  /* ********************************************************************************* */
  boolean Create_Me() override {// IDeletable
    return true;
  }
  void Delete_Me() override {// IDeletable
    this->MyBounds.Delete_Me();// wreck everything to prevent accidental re-use
    this->MyProject = null;
    this->Wipe_SubSongs();
    this->Duration = Double_NEGATIVE_INFINITY;
    this->FreshnessTimeStamp = Integer_MAX_VALUE;
  }
  void Wipe_SubSongs() {
    int len = this->SubSongs.size();
    OffsetBoxBase *obox;
    for (int cnt = 0; cnt < len; cnt++) {
      obox = this->SubSongs.at(cnt);
      delete obox;
    }
    this->SubSongs.clear();
  }
  /* ********************************************************************************* */
  JsonParse::Node* Export(CollisionLibrary& HitTable) override {// ITextable
    JsonParse::Node *phrase = new JsonParse::Node();
//    phrase->ChildrenHash = neuvo HashMap<String, JsonParse::Node>();
//    phrase->AddSubPhrase("MyName", IFactory.Utils.PackField(this->MyName));
//
//    if (false) {
//      phrase->AddSubPhrase("MaxAmplitude", IFactory.Utils.PackField(this->MaxAmplitude));// can be calculated
//      phrase->AddSubPhrase("MyBounds", MyBounds.Export(HitTable));// can be calculated
//    }
//
//    // Save my array of songlets.
//    JsonParse.Node CPointsPhrase = neuvo JsonParse.Node();
//    CPointsPhrase.ChildrenArray = IFactory.Utils.MakeArray(HitTable, this->SubSongs);
//    phrase->AddSubPhrase(this->SubSongsName, CPointsPhrase);

    return phrase;
  }
  void ShallowLoad(JsonParse::Node& phrase) {}
  void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
  /* ********************************************************************************* */
  class Group_Singer: public SingerBase {
  public:
    GroupSong* MySonglet;
    ArrayList<SingerBase*> NowPlaying;// pool of currently playing voices
    int Current_Dex = 0;
    SoundFloat Prev_Time = 0;
    int Sample_Start;
    /* ********************************************************************************* */
    Group_Singer(){ this->Create_Me(); }
    ~Group_Singer(){ this->Delete_Me(); }
    /* ********************************************************************************* */
    void Start() override {
      IsFinished = false;
      Current_Dex = 0;
      Prev_Time = 0; this->Sample_Start = 0;
      Delete_Kids();
      if (this->MySonglet->SubSongs.size() <= 0) {
        this->IsFinished = true;
      }
      if (this->InheritedMap.LoudnessFactor == 0.0) { this->IsFinished = true; }// muted, so don't waste time rendering
    }
    /* ********************************************************************************* */
    void Skip_To(SoundFloat EndTime) override {// to do: rewrite this to match bug-fixed render_to
      if (this->IsFinished) { return; }
      EndTime = this->MyOffsetBox->MapTime(EndTime);// EndTime is now time internal to GroupSong's own coordinate system
      if (this->MySonglet->SubSongs.size() <= 0) {
        this->IsFinished = true;
        this->Prev_Time = EndTime;
        return;
      }
      SoundFloat Clipped_EndTime = this->Tee_Up_Skip(EndTime);
      this->Sample_Start = Clipped_EndTime * (SoundFloat)this->SampleRate;
      int NumPlaying = NowPlaying.size();
      SingerBase *player = null;
      int cnt = 0;
      while (cnt < NumPlaying) {// then play the whole pool
        player = this->NowPlaying.at(cnt);
        player->Skip_To(Clipped_EndTime);
        cnt++;
      }
      cnt = 0;// now pack down the finished ones, probably redundant with Tee_Up_Skip now
      int AliveCnt = 0;
      for (cnt=0; cnt < NumPlaying; cnt++) {
        player = this->NowPlaying.at(cnt);
        if (player->IsFinished) {
          delete player;
        } else {
          this->NowPlaying[AliveCnt++] = player;
        }
      }
      this->NowPlaying.resize(AliveCnt);
      this->Prev_Time = EndTime;
    }
    /* ********************************************************************************* */
    void Render_To(SoundFloat EndTime, Wave& wave) override {
      if (this->IsFinished) {
        wave.Init(0, 0, this->SampleRate, 0.0);
        this->Prev_Time = EndTime;
        return;
      }

      EndTime = this->MyOffsetBox->MapTime(EndTime);// EndTime is now time internal to GroupSong's own coordinate system
      SoundFloat UnMapped_Prev_Time = this->InheritedMap.UnMapTime(this->Prev_Time);// get start time in absolute universal coordinates
      SoundFloat Clipped_EndTime = this->Tee_Up_Render(EndTime);
      SoundFloat UnMapped_EndTime = this->InheritedMap.UnMapTime(Clipped_EndTime);// get end time in absolute universal coordinates
      int Sample_End = UnMapped_EndTime * (SoundFloat)this->SampleRate;
      //wave.Init(UnMapped_Prev_Time, UnMapped_EndTime, this->SampleRate);// wave times are in parent coordinates because the parent will be reading the wave data.
      wave.Init_Sample(this->Sample_Start, Sample_End, this->SampleRate, 0.0);// wave times are in parent coordinates because the parent will be reading the wave data.

      Wave ChildWave;
      int NumPlaying = NowPlaying.size();
      SingerBase *player = null;
      int cnt = 0;
      while (cnt < NumPlaying) {// then play the whole pool
        player = this->NowPlaying.at(cnt);
        player->Render_To(Clipped_EndTime, ChildWave);
        wave.Overdub(ChildWave);// sum/overdub the waves
        cnt++;
      }
      cnt = 0;// now pack down the finished ones
      int AliveCnt = 0;
      while (cnt < NumPlaying) {
        player = this->NowPlaying[cnt];
        if (player->IsFinished) {
          delete player;
        } else {// still playing, so pack it down
          this->NowPlaying[AliveCnt++] = player;
        }
        cnt++;
      }
      this->NowPlaying.resize(AliveCnt);
      wave.Amplify(this->MyOffsetBox->LoudnessFactor);

      this->Prev_Time = EndTime;
      this->Sample_Start = Sample_End;
    }
    /* ********************************************************************************* */
    void Skip_To_Sample(int EndSample) override {
    }
    /* ********************************************************************************* */
    void Render_To_Sample(int EndSample, Wave& wave) override {
    }
    /* ********************************************************************************* */
    boolean Create_Me() override {// IDeletable
      return SingerBase::Create_Me();
    }
    void Delete_Me() override {// IDeletable
      SingerBase::Delete_Me();
      Delete_Kids();
    }
    void Delete_Kids() {// delete the whole pool of sub-singers
      int len = this->NowPlaying.size();
      SingerBase *singer;
      for (int cnt = 0; cnt < len; cnt++) {
        singer = this->NowPlaying.at(cnt);
        delete singer;
      }
      this->NowPlaying.clear();
    }
    /* ********************************************************************************* */
    SoundFloat Tee_Up_Skip(SoundFloat EndTime) {
      if (EndTime < 0) { EndTime = 0; }// clip time
      int NumSonglets = MySonglet->SubSongs.size();
      int FinalSongletDex = NumSonglets - 1;
      SoundFloat Final_Start = this->MySonglet->SubSongs.at(FinalSongletDex)->TimeX;
      Final_Start = Math::min(Final_Start, EndTime);
      SoundFloat Final_Time = this->MySonglet->Get_Duration();
      if (EndTime > Final_Time) {
        this->IsFinished = true;
        EndTime = Final_Time;// clip time
      }

      // clear out any singers that finished before EndTime
      int NumPlaying = this->NowPlaying.size();
      OffsetBoxBase *obox;
      SingerBase *singer;
      int AliveCnt = 0;
      for (int cnt=0;cnt<NumPlaying;cnt++){
        singer = this->NowPlaying[cnt];
        obox = singer->MyOffsetBox;
        SoundFloat Child_End_Time = obox->MyBounds.Max.x;
        Child_End_Time = obox->UnMapTime(Child_End_Time);
        if (Child_End_Time < EndTime){// Delete any singers whose songs end before the next 'play zone'.
          delete singer;
        }else{
          this->NowPlaying[AliveCnt++] = singer;// pack them down
        }
      }
      this->NowPlaying.resize(AliveCnt);

      while (this->Current_Dex < NumSonglets) {// first find neuvo songlets in this time range and add them to pool
        obox = MySonglet->SubSongs.at(this->Current_Dex);
        if (Final_Start < obox->TimeX) { break; }// repeat until obox start time overtakes EndTime
        SoundFloat Child_End_Time = obox->MyBounds.Max.x;
        Child_End_Time = obox->UnMapTime(Child_End_Time);
        if (EndTime < Child_End_Time){// Only use songlets that cross into the next 'play zone'
          SingerBase *singer = obox->Spawn_Singer();
          singer->Inherit(*this);
          this->NowPlaying.add(singer);
          singer->Start();
        }
        this->Current_Dex++;
      }
      return EndTime;
    }
    /* ********************************************************************************* */
    SoundFloat Tee_Up_Render(SoundFloat EndTime) {// this works best for render_to
      if (EndTime < 0) { EndTime = 0; }// clip time
      int NumSonglets = MySonglet->SubSongs.size();
      int FinalSongletDex = NumSonglets - 1;
      SoundFloat Final_Start = this->MySonglet->SubSongs.at(FinalSongletDex)->TimeX;
      Final_Start = Math::min(Final_Start, EndTime);
      SoundFloat Final_Time = this->MySonglet->Get_Duration();
      if (EndTime > Final_Time) {
        this->IsFinished = true;
        EndTime = Final_Time;// clip time
      }
      OffsetBoxBase *obox;
      while (this->Current_Dex < NumSonglets) {// first find neuvo songlets in this time range and add them to pool
        obox = MySonglet->SubSongs.at(this->Current_Dex);
        if (Final_Start < obox->TimeX) { break; }// repeat until obox start time overtakes EndTime
        SingerBase *singer = obox->Spawn_Singer();
        singer->Inherit(*this);
        this->NowPlaying.add(singer);
        singer->Start();
        this->Current_Dex++;
      }
      return EndTime;
    }
  };
  /* ********************************************************************************* */
  class Group_OffsetBox: public OffsetBoxBase {// location box to transpose in pitch, move in time, etc.
  public:
    GroupSong* Content = null;
    SoundFloat GroupScaleX = 1.0;
    String GroupScaleXName = "GroupScaleX";// for serialization
    String ObjectTypeName = "Group_OffsetBox";
    /* ********************************************************************************* */
    Group_OffsetBox() {
      OffsetBoxBase();
      this->Clear();
      this->Create_Me();
    }
    /* ********************************************************************************* */
    ~Group_OffsetBox() {
      this->Delete_Me();
    }
    /* ********************************************************************************* */
    GroupSong* GetContent() {
      return Content;
    }
    /* ********************************************************************************* */
    void Attach_Songlet(GroupSong* songlet) {// for serialization
      this->MySonglet = this->Content = songlet;
      songlet->Ref_Songlet();
    }
    /* ********************************************************************************* */
    void RescaleGroupTimeX(SoundFloat Factor) {
      this->Content->RescaleGroupTimeX(Factor);
    }
    /* ********************************************************************************* */
    void Draw_Me(IDrawingContext& ParentDC) override {// IDrawable
      OffsetBoxBase::Draw_Me(ParentDC);
      // here draw the Rescale_TimeX handle
    }
    /* ********************************************************************************* */
    Group_Singer* Spawn_Singer() override {// always always always override this
      Group_Singer *Singer = this->Content->Spawn_Singer();// for render time
      Singer->MyOffsetBox = this;// Transfer all of this box's offsets to singer.
      return Singer;
    }
    /* ********************************************************************************* */
    Group_OffsetBox* Clone_Me() override {// ICloneable, always override this thusly
      Group_OffsetBox *child = new Group_OffsetBox();// clone
      child->Copy_From(*this);
      child->Content = this->Content;
      return child;
    }
    /* ********************************************************************************* */
    Group_OffsetBox* Deep_Clone_Me(CollisionLibrary& HitTable) override {// ICloneable
      Group_OffsetBox *child = new Group_OffsetBox();// clone
      child->Copy_From(*this);
      child->Attach_Songlet(this->Content->Deep_Clone_Me(HitTable));
      return child;
    }
    /* ********************************************************************************* */
    void BreakFromHerd(CollisionLibrary& HitTable) {// for compose time. detach from my songlet and attach to an identical but unlinked songlet
      GroupSong *clone = this->Content->Deep_Clone_Me(HitTable);
      if (this->Content->UnRef_Songlet() <= 0) {
        this->Content->Delete_Me();
        delete this->Content;
      }
      this->Attach_Songlet(clone);
    }
    /* ********************************************************************************* */
    void BreakFromHerd_Shallow() {// for compose time. detach from my songlet and attach to an identical but unlinked songlet
      GroupSong *clone = this->Content->Shallow_Clone_Me();
      if (this->Content->UnRef_Songlet() <= 0) {
        this->Content->Delete_Me();
        delete this->Content;
      }
      this->Content = clone;
      this->Content->Ref_Songlet();
    }
    /* ********************************************************************************* */
    boolean Create_Me() {// IDeletable
      return true;
    }
    void Delete_Me() {// IDeletable
      OffsetBoxBase::Delete_Me();
      this->GroupScaleX = Double_NEGATIVE_INFINITY;
      //ISonglet::Unref(&(this->Content));
      if (this->Content != null) {
        if (this->Content->UnRef_Songlet() <= 0) {
          delete this->Content;
          this->Content = null;
        }
      }
    }
    /* ********************************************************************************* */
    JsonParse::Node* Export(CollisionLibrary& HitTable) {
      return nullptr;
    }
    void ShallowLoad(JsonParse::Node& phrase) {}
    void Consume(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {}
    ISonglet* Spawn_And_Attach_Songlet() {
      return nullptr;
    }
    /* ********************************************************************************* */
//    class Factory: public  IFactory {// for serialization
//    public:
//      Group_OffsetBox* Create(JsonParse::Node& phrase, CollisionLibrary& ExistingInstances) {
//        return nullptr;
//      }
//    };
  };
  /* ********************************************************************************* */
  Group_OffsetBox* Spawn_OffsetBox() override {
    Group_OffsetBox *lbox = new Group_OffsetBox();// Spawn an OffsetBox specific to this type of songlet.
    lbox->Attach_Songlet(this);
    return lbox;
  }
  /* ********************************************************************************* */
  Group_Singer* Spawn_Singer() override {
    Group_Singer *GroupPlayer = new Group_Singer();// Spawn a singer specific to this type of songlet.
    GroupPlayer->MySonglet = this;
    GroupPlayer->Set_Project(this->MyProject);// inherit project
    return GroupPlayer;
  }
};


#endif
