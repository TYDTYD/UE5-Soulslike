# Run through Unreal ProgrammaticToolset.execute_tool_script (provides execute_tool).
# Layout only: item data, equipment, and inventory action bindings are not implemented.
import json, copy
BP={"refPath":"/Game/UI/WBP_Inventory.WBP_Inventory"}
BASE=BP["refPath"]+":WidgetTree."
def call(group,name,args):
 return execute_tool(group+"."+name,json.dumps(args))
def umg(name,**args):
 return call("UMGToolSet.UMGToolSet",name,dict({"widgetBlueprint":BP},**args))
def inspect(o):
 return json.loads(call("editor_toolset.toolsets.object.ObjectTools","list_properties",{"instance":o})["returnValue"])
def get(o,keys):
 return json.loads(call("editor_toolset.toolsets.object.ObjectTools","get_properties",{"instance":o,"properties":keys})["returnValue"])
def put(o,values):
 schema=inspect(o)
 for k in values:
  if k not in schema: raise ValueError("Unknown property "+k)
 get(o,list(values))
 result=call("editor_toolset.toolsets.object.ObjectTools","set_properties",{"instance":o,"values":json.dumps(values)})
 if not result["returnValue"]: raise ValueError("Set failed "+str(o))
def ref(n): return {"refPath":BASE+n}
def col(r,g,b,a=1): return dict(r=r,g=g,b=b,a=a)
def slate(c): return dict(specifiedColor=c,colorUseRule="UseColor_Specified")
def place(slot,x,y,w,h,z=0):
 put(slot,{"layoutData":{"offsets":dict(left=x,top=y,right=w,bottom=h),"anchors":{"minimum":{"x":0,"y":0},"maximum":{"x":0,"y":0}},"alignment":{"x":0,"y":0}},"bAutoSize":False,"zOrder":z})
def add(cls,n,parent,x=0,y=0,w=100,h=30,values=None):
 a=umg("AddWidget",widgetClass={"refPath":"/Script/UMG."+cls},widgetDisplayName=n,parentWidget=parent)["returnValue"]
 inspect(a["widget"])
 if values: put(a["widget"],values)
 if a["slot"]!="None":
  schema=inspect(a["slot"])
  if "layoutData" in schema: place(a["slot"],x,y,w,h)
 return a["widget"]
WHITE=col(.88,.88,.88)
MUTED=col(.42,.42,.42)
ACCENT=col(.78,.56,.12)
def label(n,p,t,x,y,w=300,h=32,size=18,color=None):
 f=copy.deepcopy(FONT);f["size"]=size
 return add("TextBlock",n,p,x,y,w,h,{"text":t,"font":f,"colorAndOpacity":slate(color or WHITE),"visibility":"HitTestInvisible"})
def rect(n,p,x,y,w,h,c):
 return add("Image",n,p,x,y,w,h,{"colorAndOpacity":c,"visibility":"HitTestInvisible"})
def panel(n,p,x,y,w,h):
 return add("CanvasPanel",n,p,x,y,w,h,{"visibility":"SelfHitTestInvisible"})
def button(n,p,t,x,y,w=160,h=44,enabled=True):
 b=add("Button",n,p,x,y,w,h,{"widgetStyle":STYLE,"bIsEnabled":enabled})
 umg("ToggleWidgetAsVariable",widget=b,bIsVariable=True)
 l=label(n+"Label",b,t,0,0,w,h,16,WHITE if enabled else MUTED)
 put(l,{"justification":"Center"})
 return b

def shared_item(n,p,x,y,size=94):
 a=umg("AddWidget",widgetClass={"refPath":"/Game/UI/WBP_InventoryItem.WBP_InventoryItem_C"},widgetDisplayName=n,parentWidget=p)["returnValue"]
 inspect(a["widget"])
 place(a["slot"],x,y,size,size)
 return a["widget"]

def run():
 global FONT,STYLE
 names=[w["widgetName"] for w in umg("GetWidgets")["returnValue"]["widgets"]]
 FONT=get(ref("InventoryTitle" if "InventoryTitle" in names else "ScreenTitle"),["font"])["font"]
 STYLE=get(ref("CloseButton"),["widgetStyle"])["widgetStyle"]
 for state,c in [("normal",col(.035,.035,.035)),("hovered",col(.085,.085,.085)),("pressed",col(.018,.018,.018)),("disabled",col(.022,.022,.022))]:
  brush=copy.deepcopy(STYLE["normal"])
  brush["tintColor"]=slate(c)
  brush["outlineSettings"]["cornerRadii"]=dict(x=0,y=0,z=0,w=0)
  brush["outlineSettings"]["color"]=slate(col(.13,.13,.13) if state!="hovered" else ACCENT)
  STYLE[state]=brush
 for key in ["normalForeground","hoveredForeground","pressedForeground","disabledForeground"]:
  STYLE[key]=slate(MUTED if key=="disabledForeground" else WHITE)
 canvas=ref("InventoryCanvas")
 umg("MoveWidget",widget=ref("CloseButton"),newParent=canvas)
 widgets=umg("GetWidgets")["returnValue"]["widgets"]
 for item in widgets:
  if item["parent"]==canvas and item["widgetName"]!="CloseButton":
   umg("RemoveWidget",widget=item["widget"])
 rect("ScreenTint",canvas,0,0,1920,1080,col(.008,.008,.008,.98))
 label("ScreenTitle",canvas,"소지품",64,38,300,60,34)
 label("ScreenKicker",canvas,"INVENTORY  /  PERSONAL GEAR",66,94,500,25,13,MUTED)
 rect("TopRule",canvas,64,133,1792,1,col(.13,.13,.13))
 rect("ActiveTabLine",canvas,64,131,120,3,ACCENT)
 left=panel("EquipmentPanel",canvas,64,167,540,770)
 label("EquipmentHeading",left,"장비",0,0,200,40,25)
 label("EquipmentSubheading",left,"EQUIPMENT",0,42,250,25,12,MUTED)
 for i,(name,t) in enumerate([("Head","머리"),("Body","상의"),("Legs","하의"),("Bag","가방")]):
  x=(i%2)*260;y=90+(i//2)*132
  button("Equipment"+name,left,t,x,y,244,114,False)
 for i,t in enumerate(["주 무기","보조 무기"]):
  button("WeaponSlot"+str(i),left,t,i*260,370,244,105,False)
 rect("ConditionRule",left,0,510,504,1,col(.13,.13,.13))
 label("ConditionHeading",left,"캐릭터 상태",0,534,300,35,21)
 for i,t in enumerate(["체력","스태미나","소지 무게"]):
  label("ConditionName"+str(i),left,t,0,590+i*49,260,30,17,MUTED)
  label("ConditionValue"+str(i),left,"—",415,590+i*49,80,30,18)
 right=panel("StoragePanel",canvas,666,167,1190,770)
 label("PocketHeading",right,"주머니",0,0,220,40,25)
 label("PocketSubheading",right,"QUICK ACCESS",0,42,230,25,12,MUTED)
 for i in range(6):
  shared_item("PocketSlot"+str(i+1),right,i*108,88,98)
  label("PocketKey"+str(i+1),right,str(i+1),i*108+7,94,24,20,11,MUTED)
 label("BagHeading",right,"가방",0,222,230,40,25)
 label("BagCapacity",right,"BACKPACK",0,264,230,24,12,MUTED)
 button("SortButton",right,"자동 정렬",654,225,150,43,False)
 grid=panel("BagGrid",right,0,310,804,418)
 for row in range(4):
  for c in range(8):
   shared_item("BagSlot_%02d"%(row*8+c),grid,c*102,row*102,94)
 detail=panel("ItemDetailsPanel",right,844,0,346,728)
 rect("DetailBackground",detail,0,0,346,728,col(.018,.018,.018))
 rect("DetailTopRule",detail,0,0,346,2,col(.26,.26,.26))
 label("DetailHeading",detail,"아이템 정보",24,25,300,35,22)
 label("DetailEmptySymbol",detail,"+",145,160,60,70,44,MUTED)
 label("DetailEmptyTitle",detail,"선택한 아이템 없음",24,285,300,36,20)
 label("DetailEmptyDescription",detail,"가방에서 아이템을 선택하세요.",24,335,300,60,15,MUTED)
 button("UseButton",detail,"사용",24,494,298,46,False)
 button("EquipButton",detail,"장착",24,550,144,46,False)
 button("QuickSlotButton",detail,"퀵슬롯 등록",178,550,144,46,False)
 button("DropButton",detail,"버리기",24,606,298,46,False)
 rect("FooterRule",canvas,64,972,1792,1,col(.13,.13,.13))
 label("FooterHelp",canvas,"아이템 선택  ·  소지품 관리",64,1008,900,32,16,MUTED)
 widgets=umg("GetWidgets")["returnValue"]["widgets"]
 close=next(w for w in widgets if w["widgetName"]=="CloseButton")
 place(close["slot"],1656,995,200,48,10)
 put(close["widget"],{"widgetStyle":STYLE})
 put(ref("CloseHint"),{"text":"닫기  [ESC]","font":dict(FONT,size=17),"colorAndOpacity":slate(WHITE),"visibility":"HitTestInvisible"})
 return {"compile":umg("CompileWidgetBlueprint"),"widgets":len(widgets)}


