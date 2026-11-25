# Rebuild the existing /Game/UI/WBP_InteractInventory through ProgrammaticToolset.
# Requires WBP_InventoryItem and WBP_Inventory; preserves the CloseButton event.
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


def expose(w):
 umg("ToggleWidgetAsVariable",widget=w,bIsVariable=True)
 return w
def item(n,p,x,y,size=74):
 a=umg("AddWidget",widgetClass={"refPath":"/Game/UI/WBP_InventoryItem.WBP_InventoryItem_C"},widgetDisplayName=n,parentWidget=p)["returnValue"]
 inspect(a["widget"])
 place(a["slot"],x,y,size,size)
 return a["widget"]
def grid(n,p,x,y,cols,rows,count):
 g=expose(panel(n,p,x,y,cols*78,rows*78))
 for i in range(count):item(n+"_Item_%02d"%i,g,(i%cols)*78,(i//cols)*78)
 return g

def run():
 global BP,BASE,FONT,STYLE
 FONT=get(ref("ScreenTitle"),["font"])["font"]
 STYLE=get(ref("CloseButton"),["widgetStyle"])["widgetStyle"]
 BP={"refPath":"/Game/UI/WBP_InteractInventory.WBP_InteractInventory"};BASE=BP["refPath"]+":WidgetTree."
 canvas=ref("InventoryCanvas")
 for w in umg("GetWidgets")["returnValue"]["widgets"]:
  if w["parent"]==canvas and w["widgetName"]!="CloseButton":umg("RemoveWidget",widget=w["widget"])
 rect("ScreenTint",canvas,0,0,1920,1080,col(.008,.008,.008,.98))
 label("ScreenTitle",canvas,"소지품",64,42,320,52,32)
 label("ScreenKicker",canvas,"INVENTORY",66,98,320,22,12,MUTED)
 rect("TopRule",canvas,64,138,1792,1,col(.13,.13,.13))
 rect("ActiveTabLine",canvas,64,136,100,3,ACCENT)
 equip=panel("EquipmentPanel",canvas,64,182,344,720)
 label("EquipmentHeading",equip,"장비",0,0,300,36,24)
 for i,t in enumerate(["머리","상의","하의","가방"]):
  button("EquipmentSlot"+str(i),equip,t,(i%2)*174,64+(i//2)*130,160,116,False)
 button("PrimaryWeapon",equip,"주 무기",0,338,334,90,False)
 button("SecondaryWeapon",equip,"보조 무기",0,440,334,90,False)
 label("ConditionHeading",equip,"캐릭터 상태",0,568,300,32,21)
 for i,t in enumerate(["체력","스태미나","소지 무게"]):
  label("ConditionLabel"+str(i),equip,t,0,618+i*38,240,28,16,MUTED)
  expose(label("ConditionValue"+str(i),equip,"—",290,618+i*38,44,28,16))
 rect("PlayerDivider",canvas,438,182,1,730,col(.12,.12,.12))
 player=panel("PlayerInventoryPanel",canvas,478,182,624,730)
 label("PlayerInventoryHeading",player,"내 소지품",0,0,400,40,25)
 expose(label("PlayerWeightText",player,"— kg",484,5,140,32,18,MUTED))
 rect("PlayerHeaderRule",player,0,52,620,1,col(.17,.17,.17))
 label("PocketHeading",player,"주머니",0,82,300,34,20)
 pocket=expose(panel("PlayerPocketGrid",player,0,130,624,96))
 for i in range(6):
  item("PlayerPocketItem"+str(i),pocket,i*104,0,96)
  label("PocketKey"+str(i),pocket,str(i+1),i*104+7,6,26,20,11,MUTED)
 label("BagHeading",player,"가방",0,260,300,34,22)
 expose(label("PlayerCapacityText",player,"— / —",356,264,100,28,16,MUTED))
 button("PlayerSortButton",player,"정렬",492,254,128,40,False)
 grid("PlayerItemsGrid",player,0,316,8,4,32)
 label("PlayerEmptyHint",player,"가방에서 옮길 아이템을 선택하세요.",0,656,620,28,15,MUTED)
 rect("TargetDivider",canvas,1142,182,1,730,col(.12,.12,.12))
 target=panel("TargetInventoryPanel",canvas,1182,182,674,730)
 expose(label("TargetNameText",target,"금고",0,0,400,40,25))
 expose(label("TargetCapacityText",target,"— / —",490,5,130,32,18,MUTED))
 rect("TargetHeaderRule",target,0,52,674,1,col(.26,.26,.26))
 expose(label("SearchStateText",target,"보관함",0,82,620,32,18,MUTED))
 grid("TargetItemsGrid",target,0,130,8,6,48)
 expose(label("TargetEmptyHint",target,"획득할 아이템을 선택하세요.",0,626,620,28,15,MUTED))
 button("TakeSelectedButton",target,"선택 획득",0,680,190,46,False)
 button("TakeAllButton",target,"모두 획득",204,680,220,46,False)
 button("DepositButton",target,"보관",438,680,182,46,False)
 rect("FooterRule",canvas,64,965,1792,1,col(.13,.13,.13))
 label("FooterHelp",canvas,"수색  /  소지품과 보관함 사이에서 아이템 이동",64,1008,1300,30,16,MUTED)
 close=next(w for w in umg("GetWidgets")["returnValue"]["widgets"] if w["widgetName"]=="CloseButton")
 place(close["slot"],1656,995,200,48,10)
 put(ref("CloseHint"),{"text":"닫기  [ESC]"})
 return {"compile":umg("CompileWidgetBlueprint"),"sharedItems":86}

