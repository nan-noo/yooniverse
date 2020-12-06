import pandas as pd
import numpy as np
from konlpy.tag import Okt
import gensim
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

data["abscon"] = data["abscon"].apply(lambda row: oktTokenizer(row))
print(data)

tokenized = data["abscon"]
#tokenized.to_excel("./finaldata/0909token1.xls") 

#lda
id2word = gensim.corpora.Dictionary(tokenized)

wordlist = []
for i in range(len(id2word)):
    #print(id2word[i])
    wordlist.append(id2word[i])
#print(wordlist)
seriesWordlist = pd.Series(wordlist)
#seriesWordlist.to_excel("./finaldata/0909wordlist1.xls") 

corpus=[id2word.doc2bow(text) for text in tokenized]
#print("id2word for each document : ", corpus)
print("# words in total : ", len(id2word))
print("# documents : ", len(corpus))

optimal_model = gensim.models.ldamodel.LdaModel(corpus=corpus, 
                                                id2word=id2word, 
                                                num_topics=10,
                                                random_state=100,
                                                update_every=1,
                                                iterations=1000,
                                                chunksize=100,
                                                passes=10,
                                                alpha='auto',
                                                eta='auto',
                                                per_word_topics=True)
model_topics = optimal_model.show_topics(formatted=False)
pprint(optimal_model.print_topics(num_words=20))

proportion = []
for i, row_list in enumerate(optimal_model[corpus]):
    #print("document number : ", i)
    row = row_list[0]
    #print(row)
    proportion.append(row)
print(pd.DataFrame(proportion))


proportion = []
for i, row_list in enumerate(optimal_model[corpus]):
    #print("document number : ", i)
    row = row_list[0]
    #print(row)
    inner_proportion = ["-", "-", "-", "-", "-", "-", "-", "-", "-", "-"]
    for j, (topic_num, prop) in enumerate(row):
        for k in range(10):
            if topic_num == k:
                inner_proportion[k] = prop
    proportion.append(inner_proportion)
    #print(inner_proportion)
#print(proportion)

#topic_proportions = pd.DataFrame(data["title"])

topic_proportions = pd.concat([data["title"], pd.DataFrame(proportion)], axis=1)
topic_proportions.columns = ["Title", "T0", "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9"]
print(topic_proportions)
topic_proportions.to_excel("./finaldata/0909_topic_proportions_total.xlsx")

    