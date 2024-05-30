import maya.cmds as cmds
import struct
from enum import IntEnum


#================================================================
# マクロ定義
#================================================================
ATTR_NUM     = 9
WINDOW_NAME  = "AnimationConverter"
WINDOW_TITLE = "Animation Converter ver.1.10"
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
# キーフレームデータ格納用
nodes = []
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
# キーフレームデータの取得
#----------------------------------------------------------------
def GetKeyframeData(self):
    # オブジェクトと属性の指定
    #obj = "MIKU_HEAD"
    obj = selectedObj
    #attr = "rotateX"
    # フレームレンジの取得
    sFrame = int(cmds.playbackOptions(q=True, min=True))
    eFrame = int(cmds.playbackOptions(q=True, max=True))
    # リストの要素をリセットする
    nodes.clear()
    # キーフレームが存在するか確認
    kFrame = cmds.keyframe(obj, attribute="rotateX", query=True)
    if kFrame:
        # 各アトリビュートのキーの時間と値を取得する
        for attr in attrs:
            nodes.extend(cmds.keyframe(obj, attribute=attr, time=(sFrame,eFrame), query=True, valueChange=True, timeChange=True))
        print("キーフレームデータを更新しました -> {}".format(obj))
        #PrintNodes(self)
    else:
        for attr in attrs:
            nodes.extend([0.0, 0.0])
        print("キーフレームデータが見つかりませんでした -> 空データで更新しました")
        #PrintNodes(self)
 
#----------------------------------------------------------------
# リストの要素の出力
#----------------------------------------------------------------
def PrintNodes(self):
    for node in nodes:
        print(node)
    print(len(nodes))

#----------------------------------------------------------------
# リストの要素をテキストファイルに書き込む
#----------------------------------------------------------------
def WriteText(self):
    # 現在のシーンディレクトリを取得する
    currentDir = GetCurrentDir(self)
    # ファイル名が入力されていない場合、処理を中断
    global fileName
    if fileName == "":
        print("ファイル名を入力してください")
        return
    # キーフレームデータがない場合、処理を中断
    if len(nodes) == 0:
        print("キーフレームデータが見つかりませんでした")
        return
    # 書き込むファイル名の拡張子をテキストファイルにする
    outputName = fileName + ".txt"
    # ファイルパスを構築
    filePath = currentDir + outputName
    # ファイルを書き込みモードで開く
    with open(filePath, "w") as file:
        keyNum = int(len(nodes) / ATTR_NUM)
        # リストの要素を書き込んでいく
        for i in range(0, keyNum, 2):
            time = 0
            if i == keyNum - 2:
                time = 1.0
            else:
                time = nodes[ATTR.TX * keyNum + i + 2] - nodes[ATTR.TX * keyNum + i]
            file.write("tx:{} ty:{} tz:{} rx:{} ry:{} rz:{} sx:{} sy:{} sz:{} frame:{}\n".format( 
            str(round(nodes[ATTR.TX * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.TY * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.TZ * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.RX * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.RY * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.RZ * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.SX * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.SY * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.SZ * keyNum + i + 1], 3)),
            str(time),
            ))
        print("書き込みに成功しました -> {}".format(outputName))

#----------------------------------------------------------------
# 全パーツをテキストファイルでまとめて出力
#----------------------------------------------------------------
def WriteAllText(self):
    sceneObjs = cmds.ls(charName+"*", transforms=True)
    # オブジェクト数が0より大きい場合に処理
    if len(sceneObjs) > 0:
        print("\nテキストファイルでまとめて出力")
        for sObj in sceneObjs:
            # キーフレームデータの取得
            global selectedObj
            selectedObj = sObj
            GetKeyframeData(self)
            # ファイル書き込み
            global fileName
            fileName = sObj
            WriteText(self)
            
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
    # キーフレームデータがない場合、処理を中断
    if len(nodes) == 0:
        print("キーフレームデータが見つかりませんでした")
        return
    # 書き込むファイル名の拡張子をテキストファイルにする
    outputName = fileName + ".csv"
    # ファイルパスを構築
    filePath = currentDir + outputName
    # ファイルを書き込みモードで開く
    with open(filePath, "w") as file:
        keyNum = int(len(nodes) / ATTR_NUM)
        # リストの要素を書き込んでいく
        file.write("tx,ty,tz,rx,ry,rz,sx,sy,sz,frame\n")
        for i in range(0, keyNum, 2):
            time = 0
            if i == keyNum - 2:
                time = 1.0
            else:
                time = nodes[ATTR.TX * keyNum + i + 2] - nodes[ATTR.TX * keyNum + i]
            file.write("{},{},{},{},{},{},{},{},{},{}\n".format( 
            str(round(nodes[ATTR.TX * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.TY * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.TZ * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.RX * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.RY * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.RZ * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.SX * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.SY * keyNum + i + 1], 3)),
            str(round(nodes[ATTR.SZ * keyNum + i + 1], 3)),
            str(time),
            ))
        print("書き込みに成功しました -> {}".format(outputName))

#----------------------------------------------------------------
# 全パーツをCSVファイルでまとめて出力
#----------------------------------------------------------------
def WriteAllCSV(self):
    sceneObjs = cmds.ls(charName+"*", transforms=True)
    # オブジェクト数が0より大きい場合に処理
    if len(sceneObjs) > 0:
        print("\nCSVファイルでまとめて出力")
        for sObj in sceneObjs:
            # キーフレームデータの取得
            global selectedObj
            selectedObj = sObj
            GetKeyframeData(self)
            # ファイル書き込み
            global fileName
            fileName = sObj
            WriteCSV(self)  

#----------------------------------------------------------------
# リストの要素をバイナリファイルに書き込む ※ただいま放置中(´・ω・`)
#----------------------------------------------------------------
def WriteBinary(self):
    # 現在のプロジェクトディレクトリを取得する
    currentDir = "C:\\Users\\ths30329\\Desktop"
    # 書き込むファイル名
    fileName = "keyframe.bin"
    # ファイルパスを構築
    filePath = os.path.join(currentDir, fileName)
    # ファイルをバイナリ書き込みモードで開く
    with open(filePath, "wb") as file:
        # リストの要素を書き込んでいく
        for node in nodes:
            # 要素をバイナリデータに変換
            binaryData = struct.pack('f', node)
            file.write(binaryData)
        print("書き込みに成功しました")

#----------------------------------------------------------------
# ドロップダウンリストの値を取得
#----------------------------------------------------------------
def GetDropdownListObj(self):
    selectedItem = cmds.optionMenu(DROPDOWN_OBJ, query=True, value=True)
    #print("選択されたアイテム:", selected_item)
    global selectedObj
    selectedObj = selectedItem
    #print(selectedObj)
    GetKeyframeData(self)

#----------------------------------------------------------------
# ドロップダウンリストのアイテムを更新
#----------------------------------------------------------------
def UpdateDropdownList(self):
    # ドロップダウンリストのアイテムをリセット
    cmds.optionMenu(DROPDOWN_OBJ, edit=True, deleteAllItems=True)
    # ドロップダウンリストにアイテムを追加
    sceneObjs = cmds.ls(charName+"*", transforms=True)
    for sObj in sceneObjs:
        cmds.menuItem(label=sObj, parent=DROPDOWN_OBJ)
    if len(sceneObjs) > 0:
        global selectedObj
        selectedObj = sceneObjs[0]
        GetKeyframeData(self)

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
# テキストボックスの値を取得(キャラクター名)
#----------------------------------------------------------------
def GetTextBoxChar(self):
    textValue = cmds.textField(TEXT_CHAR, query=True, text=True)
    #print("テキストボックスの値:", text_value)
    global charName
    charName = textValue
    #print(fileName)
    print("キャラクター名を更新しました -> {}".format(charName))
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
# テキストボックスの作成(キャラクター名)
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=1, height=50, columnAttach=[1,"both",75])
cmds.textField(TEXT_CHAR, 
               width=150, height=30, 
               placeholderText="ここにキャラクター名を入力", 
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
cmds.rowLayout(numberOfColumns=3, 
               height=50, 
               columnWidth3=(80, 110, 110), 
               columnAttach=([1,"both",0], [2,"both",20]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetKeyframeData)
cmds.text(label="1つだけ出力:", font="boldLabelFont")
button1 = cmds.button(label="TXTファイル", command=WriteText)
button2 = cmds.button(label="CSVファイル", command=WriteCSV)
# レイアウトの終了
cmds.setParent('..')

#----------------------------------------------------------------
# ボタンの作成 ※全出力
#----------------------------------------------------------------
cmds.rowLayout(numberOfColumns=3, 
               height=50, 
               columnWidth3=(80,110,110), 
               columnAttach=([1,"both",0], [2,"both",20]))
# ボタンの追加
#button1 = cmds.button(label="データ更新", command=GetKeyframeData)
cmds.text(label="まとめて出力:", font="boldLabelFont")
button3 = cmds.button(label="TXTファイル", command=WriteAllText)
button4 = cmds.button(label="CSVファイル", command=WriteAllCSV)
# レイアウトの終了
cmds.setParent('..')

# ウィンドウの表示
cmds.showWindow(windowName)




