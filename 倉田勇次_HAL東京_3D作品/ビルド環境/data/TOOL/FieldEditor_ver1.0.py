import maya.cmds as cmds
import struct
from enum import IntEnum


#================================================================
# マクロ定義
#================================================================
ATTR_NUM     = 9
WINDOW_NAME  = "FieldEditor"
WINDOW_TITLE = "FieldEditor ver.1.0"
DROPDOWN_OBJ = "DropdownObj"
TEXT_CHAR    = "TextCharacter"
TEXT_OUTPUT  = "TextOutput"
CHAR_NAME    = ""
class ATTR(IntEnum):
    TX = 0
    TY = 1
    TZ = 2
    RX = 3
    RY = 4
    RZ = 5
    SX = 6
    SY = 7
    SZ = 8


#================================================================
# グローバル変数
#================================================================
# データ格納用
nodes = []
#検索オブジェクトリスト
sceneObjs = []
# 取得アトリビュート
attrs = [
        "tx", "ty", "tz", 
        "rx", "ry", "rz",
        "sx", "sy", "sz"
        ]
# 選択しているオブジェクト名
selectedObj = ""
fileName    = ""
charName    = CHAR_NAME


#================================================================
# 関数
#================================================================

#----------------------------------------------------------------
# 現在のシーンのディレクトリの取得
#----------------------------------------------------------------
def GetCurrentDir(self):
    # 現在のシーンのファイルパスを取得
    scenePath = cmds.file(query=True, sceneName=True)
    pathList = scenePath.split("/")
    sceneName = pathList[-1]
    
    if scenePath:
        # ファイルパスからディレクトリを抽出
        sceneDir = scenePath.replace(sceneName, '')
        return sceneDir
    else:
        return None

#----------------------------------------------------------------
# データの取得
#----------------------------------------------------------------
def GetData(self):
    # オブジェクトと属性の指定
    #obj = "MIKU_HEAD"
    obj = selectedObj
    #attr = "rotateX"

    # リストの要素をリセットする
    nodes.clear()

    # 各オブジェクトのpos, rot, sclを取得
    for attr in attrs:
        attr_full_name = obj + "." + attr
        value = cmds.getAttr(attr_full_name)
        if isinstance(value, (list, tuple)):
            nodes.extend(value)
            #print("追加された要素:", value)
        else:
            nodes.append(value)
            #print("追加された要素:", value)
    print("モデルデータを更新しました -> {}".format(obj))
    #PrintNodes(self)
 
#----------------------------------------------------------------
# リストの要素の出力
#----------------------------------------------------------------
def PrintNodes(self):
    for node in nodes:
        print(node)
    print(len(nodes))
            
#----------------------------------------------------------------
# リストの要素をCSVファイルに書き込む
#----------------------------------------------------------------
def WriteCSV(self):
    # 現在のシーンディレクトリを取得する
    currentDir = GetCurrentDir(self)
    # ファイル名が入力されていない場合、処理を中断
    global fileName
    if fileName == "":
        print("ファイル名を入力してください")
        return
    # 書き込むファイル名の拡張子をテキストファイルにする
    outputName = fileName + ".csv"
    # ファイルパスを構築
    filePath = currentDir + outputName
    # ファイルを書き込みモードで開く
    with open(filePath, "w") as file:
        # リストの要素を書き込んでいく
        file.write("tx,ty,tz,rx,ry,rz,sx,sy,sz\n")
        # オブジェクトごとにデータを書き込む
        global sceneObjs
        global selectedObj
        for obj in sceneObjs:
            selectedObj = obj
            GetData(self)
            for i in range(0, len(nodes), ATTR_NUM):
                file.write("{},{},{},{},{},{},{},{},{}\n".format( 
                str(round(nodes[ATTR.TX + i], 3)),
                str(round(nodes[ATTR.TY + i], 3)),
                str(round(nodes[ATTR.TZ + i], 3)),
                str(round(nodes[ATTR.RX + i], 3)),
                str(round(nodes[ATTR.RY + i], 3)),
                str(round(nodes[ATTR.RZ + i], 3)),
                str(round(nodes[ATTR.SX + i], 3)),
                str(round(nodes[ATTR.SY + i], 3)),
                str(round(nodes[ATTR.SZ + i], 3)),
                ))
        print("書き込みに成功しました -> {}".format(outputName))

#----------------------------------------------------------------
# 全オブジェクトをCSVファイルでまとめて出力
#----------------------------------------------------------------
def WriteAllCSV(self):
    global sceneObjs
    sceneObjs = cmds.ls(charName+"*", transforms=True)
    # オブジェクト数が0より大きい場合に処理
    if len(sceneObjs) > 0:
        print("\nCSVファイルでまとめて出力")
        for sObj in sceneObjs:
            # データの取得
            global selectedObj
            selectedObj = sObj
            GetData(self)
            # ファイル書き込み
            global fileName
            fileName = sObj
            WriteCSV(self)  

#----------------------------------------------------------------
# ドロップダウンリストの値を取得
#----------------------------------------------------------------
def GetDropdownListObj(self):
    selectedItem = cmds.optionMenu(DROPDOWN_OBJ, query=True, value=True)
    #print("選択されたアイテム:", selected_item)
    global selectedObj
    selectedObj = selectedItem
    #print(selectedObj)
    GetData(self)

#----------------------------------------------------------------
# ドロップダウンリストのアイテムを更新
#----------------------------------------------------------------
def UpdateDropdownList(self):
    # ドロップダウンリストのアイテムをリセット
    cmds.optionMenu(DROPDOWN_OBJ, edit=True, deleteAllItems=True)
    # ドロップダウンリストにアイテムを追加
    global sceneObjs
    sceneObjs = cmds.ls(charName+"*", transforms=True)
    for sObj in sceneObjs:
        cmds.menuItem(label=sObj, parent=DROPDOWN_OBJ)
    if len(sceneObjs) > 0:
        global selectedObj
        selectedObj = sceneObjs[0]
        GetData(self)

#----------------------------------------------------------------
# テキストボックスの値を取得(出力ファイル名)
#----------------------------------------------------------------
def GetTextBoxOutput(self):
    textValue = cmds.textField(TEXT_OUTPUT, query=True, text=True)
    #print("テキストボックスの値:", text_value)
    global fileName
    fileName = textValue
    #print(fileName)
    print("出力ファイル名を更新しました -> {}".format(fileName))

#----------------------------------------------------------------
# テキストボックスの値を取得(オブジェクト名)
#----------------------------------------------------------------
def GetTextBoxChar(self):
    textValue = cmds.textField(TEXT_CHAR, query=True, text=True)
    #print("テキストボックスの値:", text_value)
    global charName
    charName = textValue
    #print(fileName)
    print("オブジェクト名を更新しました -> {}".format(charName))
    UpdateDropdownList(self)


#================================================================
# ウィンドウ
#================================================================
# ウィンドウの作成
windowName = WINDOW_NAME
if cmds.window(windowName, exists=True):
    cmds.deleteUI(windowName, window=True)
    
cmds.window(windowName, title=WINDOW_TITLE, widthHeight=[300, 300])


#================================================================
# UI
#================================================================
# UI要素の追加
#cmds.columnLayout(adjustableColumn=True)
cmds.columnLayout(columnAttach=["both",5], adjustableColumn=True)
cmds.text(label="OHAO!!\n※まとめて出力は上書きに注意してね！", height=50, backgroundColor=[1.0,1.0,1.0], font="boldLabelFont")

#----------------------------------------------------------------
# テキストボックスの作成(オブジェクト名)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",75])
cmds.textField(TEXT_CHAR, 
               width=150, height=30, 
               placeholderText="ここにオブジェクト名を入力", 
               changeCommand=GetTextBoxChar)
cmds.setParent('..')

#----------------------------------------------------------------
# ドロップダウンリストの作成
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",20])
cmds.optionMenu(DROPDOWN_OBJ, 
                label="パーツを選択してください",
                changeCommand=GetDropdownListObj)
cmds.setParent('..')

#----------------------------------------------------------------
# テキストボックスの作成(出力ファイル名)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",75])
cmds.textField(TEXT_OUTPUT, 
               width=150, height=30, 
               placeholderText="ここに出力ファイル名を入力", 
               changeCommand=GetTextBoxOutput)
cmds.setParent('..')

#----------------------------------------------------------------
# ボタンの作成 ※単体出力
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, 
               height=50, 
               columnWidth2=(80, 150), 
               columnAttach=([1,"both",0], [2,"both",20]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetData)
cmds.text(label="1つだけ出力:", font="boldLabelFont")
button2 = cmds.button(label="CSVファイル", command=WriteCSV)
# レイアウトの終了
cmds.setParent('..')

#----------------------------------------------------------------
# ボタンの作成 ※全出力
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=2, 
               height=50, 
               columnWidth2=(80,150), 
               columnAttach=([1,"both",0], [2,"both",20]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetData)
cmds.text(label="まとめて出力:", font="boldLabelFont")
button4 = cmds.button(label="CSVファイル", command=WriteAllCSV)
# レイアウトの終了
cmds.setParent('..')

# ウィンドウの表示
cmds.showWindow(windowName)




