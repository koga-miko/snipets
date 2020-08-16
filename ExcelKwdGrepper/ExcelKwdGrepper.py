import pandas as pd
import os
import re

# 最終的に保存する検索結果のファイルのタイトル
# (検索キーワードをもつエクセルファイル名、シート名、タイトル名、キーワード名、検索ヒットした箇所、テキスト内容)
columns_list = ['Excel file','Sheet name','Title','Keyword','Matched locate', 'Text']
# 設定ファイルをオープン
cfg_df = pd.read_excel('config.xlsx')
# 検索結果リストのDataFrame作成
searched_list_df = pd.DataFrame( columns=columns_list )
# 設定ファイルに記述される定義行数分ループ
for cfg_idx in range(len(cfg_df)):
    cfg_row = cfg_df.iloc[cfg_idx]
    # 設定ファイルの各行の情報を一時変数へコピー
    # （検索キーワードを持つエクセルファイル名、シート名、アイテムのタイトルが記述される列
    excel_file_name = cfg_df.iloc[cfg_idx]["excel_file_name"]                  # エクセルファイル名
    excel_sheet_name = cfg_df.iloc[cfg_idx]["sheet_name"]                      # シート名
    item_title = cfg_df.iloc[cfg_idx]["item_title"]                            # アイテムのタイトル記述列のタイトル
    keyword_title = cfg_df.iloc[cfg_idx]["keyword_title"]                      # アイテムの検索キーワード記述列のタイトル
    source_path =  cfg_df.iloc[cfg_idx]["target_source_path"]                  # 検索対象ファイルを含む最上位フォルダ
    # 検索キーワードが記述されるエクセルファイル・シートの読み込み
    df=pd.read_excel(excel_file_name, sheet_name=excel_sheet_name)
    # 検索対象フォルダ内のファイルごとに処理を実施
    for curDir, dirs, files in os.walk(source_path):
        for file in files:
            fullpath = os.path.join(curDir, file)
            # 検索するテキストファイルのオープン
            with open(fullpath,"r",encoding="utf-8")as f:
                # テキストファイルを1行ずつ処理
                for txt_idx, row in enumerate(f):
                    row = row.strip()
                    # （テキスト1行ずつに対して）各検索キーワードが含まれていないか確認する
                    for itm_idx, keycell in enumerate(df[keyword_title]):
                        if keycell:
                            # 検索キーワードは1行内に複数ある場合があるので分解する
                            for key in keycell.split("\n"):
                                if key:
                                    m = re.search(r'\b'+key+r'\b', row, flags=re.IGNORECASE)
                                    # 検索ヒットした場合
                                    if m:
                                        # 一時的に（1行あたりの情報用の）Seriesを作って検索結果リストへ追加
                                        tmp_se = pd.Series( [ os.path.basename(excel_file_name),
                                                             excel_sheet_name,
                                                             df.iloc[itm_idx][item_title],
                                                             key,
                                                             fullpath+"("+str(txt_idx+1)+"):",
                                                             row
                                                            ], index=searched_list_df.columns )
                                        searched_list_df = searched_list_df.append( tmp_se, ignore_index=True )

# 検索結果リストをソートしてインデックスをリセットする（Index列は追加しない[drop=True]）
output_df = searched_list_df.sort_values(by=columns_list).reset_index(drop=True)
# 検索結果リストをエクセルファイルへ出力
output_df.to_excel("searchd_list.xlsx")
