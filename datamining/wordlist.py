import pandas as pd
import numpy as np
from konlpy.tag import Okt
import gensim
import pyLDAvis.gensim
import numpy as np
import logging
from gensim.models.coherencemodel import CoherenceModel
import matplotlib.pyplot as plt
import time
from pprint import pprint

data = pd.read_csv("./modi_data/kor_full.csv") ############
#data = data.drop(['Unnamed: 0'], axis=1)
#print(data)


dfWordList = pd.read_excel("./word_cor.xlsx")
#print(dfWordList)

dfWordDel = dfWordList[dfWordList["수정"] == "삭제"]
dfWordMod = dfWordList[dfWordList["수정"] != "삭제"]
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
                word = word.lower()
            if word in modiword:
                word = modiword2[modiword.index(word)]
            list.append(word)        
    return list

tokenized = data["keyabscon"].apply(lambda row: oktTokenizer(row))
#print(tokenized)
tokenized.to_excel("./finaldata/1105/1105token_full.xls") ##############
print("========= tokenization completed =========")

#lda
id2word = gensim.corpora.Dictionary(tokenized)

wordlist = []
for i in range(len(id2word)):
    #print(id2word[i])
    wordlist.append(id2word[i])
#print(wordlist)
seriesWordlist = pd.Series(wordlist)
seriesWordlist.to_excel("./finaldata/1105/1105wordlist_full.xls") #################

corpus=[id2word.doc2bow(text) for text in tokenized]
#print("id2word for each document : ", corpus)
print("# words in total : ", len(id2word))
print("# documents : ", len(corpus))
