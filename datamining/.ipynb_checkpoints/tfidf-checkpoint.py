import pandas as pd
import numpy as np
from konlpy.tag import Okt
from gensim import corpora, models
from pprint import pprint

start = 1998
year = [start + i for i in range(23)]
sortedresult = pd.DataFrame()
for i in range(23):
    print("==== "+str(year[i])+" ====")
    data = pd.read_csv("./finaldata/abscon"+str(year[i])+".csv")
    data = data.drop(['Unnamed: 0'], axis=1)
    
    dfWordList = pd.read_excel("./단어수정.xlsx")
    #print(dfWordList)

    dfWordDel = dfWordList[dfWordList["수정"] == "삭제"] #words to delete
    dfWordMod = dfWordList[dfWordList["수정"] != "삭제"] #words to modify
    #print(dfWordMod)

    seriesDelete = dfWordDel["원본"]
    stopword = []
    for word in seriesDelete.values:
        stopword.append(word)
    #print(listDelete)

    seriesModify = dfWordMod["원본"]
    modiword = []
    for word in seriesModify.values:
        modiword.append(word)
    #print(len(modiword))

    seriesModify2 = dfWordMod["수정"]
    modiword2 = []
    for word in seriesModify2.values:
        modiword2.append(word)
    #print(len(modiword2))


    okt = Okt()
    def oktTokenizer(raw, stopword=stopword, pos=['Noun', 'Alpha']):
        list = []
        for word, tag in okt.pos(raw, #raw data
                                     norm=True, #normalize
                                     stem=True #stemming
                                     ):
            if len(word) > 1 and tag in pos and word not in stopword: 
                if tag == 'Alpha':
                    word = word.lower() #lowercase
                if word in modiword: #for words in modiword
                    word = modiword2[modiword.index(word)] #replace them to modiwords2
                list.append(word)        
        return list

    tokenized = data["abscon"].apply(lambda row: oktTokenizer(row))
    #print(tokenized)
    #tokenized.to_excel("./finaldata/0911token_full.xls") ##############
    print("========= tokenization completed =========")

    id2word = corpora.Dictionary(tokenized)
    corpus=[id2word.doc2bow(text) for text in tokenized]
    print("# words in total : ", len(id2word))
    print("# documents : ", len(corpus))

    #tfidf
    print("==== calculating tfidf ====")
    tfidf = models.TfidfModel(corpus)

    #tfidf per doc
    tfidflist = []
    for doc in tfidf[corpus]:
        inner_list = [0]*len(id2word) 
        for id, freq in doc:
            inner_list[id] = np.around(freq, decimals=2) #put tfidf value in the place matching its index
        tfidflist.append(inner_list)
    #print(len(tfidflist))

    tfidf_df = pd.DataFrame(tfidflist)
    tfidf_df.columns = [id2word[i] for i in range(len(id2word))] #set columns' names as words
    #print(tfidf_df)

    total_df = pd.concat([data[["year", "no", "title"]], tfidf_df], axis=1)
    #print(total_df)
    total_df.to_excel("./finaldata/0917_3/tfidf"+str(year[i])+".xlsx") 

    #sum of tfidf for each word
    columnsum = pd.DataFrame(total_df.sum(axis=0)).T
    columnsum = columnsum.drop(['no', 'title'], axis=1)
    columnsum['year'] = year[i]
    #print(columnsum)
    columnsum.to_excel("./finaldata/0917_3/sum"+str(year[i])+".xlsx")

    #sort tfidf value in descending order
    columnsum = columnsum.sort_values(by=0, axis=1, ascending=False)
    print(columnsum)
    columnsum.to_excel("./finaldata/0917_3/sorted"+str(year[i])+".xlsx")

    print("==== completed ====")

    