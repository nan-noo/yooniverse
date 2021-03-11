import time
from bs4 import BeautifulSoup as bs
from selenium import webdriver
import pandas as pd


# 4099 page까지 있음. -> 4130 page까지임 
# 100 + i = 4100 이니까
# 마지막 i = 4000이다. 그때의 helper ==>  4000 = (100 * (x-1)) + 1,  3999 = (100x - 100), 3899 = 100x, x = 38.99이니까, helper 값을 38로 두고
## helper가 38일 때(즉, 엑셀 파일 38개쨰 만들 때), i는 3701임. ==> for page in(3701, 3801), 즉 3800까지 가져왓음.
### 나머지 page (3801 ~ 4099) 더 가져와야함.

# range(1, 4100)
# 401 ~ 600페이지 까지 -> 글자수 제한 에러뜸 다시해야함...

#   helper 설명
#   helper = 1
    # i = 1, 100 + 1
    #
    # helper = 2
    # i = 101, 100 + 101
    #
    # total_page = 3
    # i = 201, 100 + 201
    #
    # i = (100 * (total_page - 1)) + 1

    #helper = 40
    # i = 3901, 4001

    #helper = 41
    # i = 4001, 4101
    #원래 => i = (100 * (helper - 1)) + 1

#놓친 페이지 1988, 2079
#for helper in range(1, 41):
for helper in range(1, 2):
    driver = webdriver.Chrome("./chromedriver")

    # 지금 계속 시간이 많이 먹히는 이유가, 기존의 코드로 진행하면, 아래의 배열 무수히 커진다, 특히 contents에 들어있는 문자열은 10^6 스케일이 되기에 크롤링이 느려진다. contents.length 계산도 루프를 돌 때마다 해야하므로 프로그램 자체가 무거워지는 것도 한몫함.
    # 그러므로 100페이지 마다 따로 엑셀 파일을 만든다. 그렇게 하면 배열을 초기화하여 사용할 수 있다.
    # 4100 페이지이므로 약 40개의 엑셀 파일 생성을 자동화한다. 마지막에 엑셀을 병합시키면된다.
    titles = []
    paths = []
    contents = []
    dates = []
    nums = []
    url = ""

    i = (100 * (helper - 1)) + 1
    #원래 => for page in range(i, 100+i):
    for page in range(i, 100+i):
        if page == 4100: #page는 4099 까지밖에 없음.
            break
        url = 'https://www1.president.go.kr/petitions/?c=42&only=2&page={}&order=1'.format(page)
        driver.get(url)
        print("current page : ", page)

        # driver.get(url)로 서버에게 get 요청을 보냈으니 response를 기다려야함. html을 받는데 걸리는 최소시간 -> 1.5s
        time.sleep(1.5)
        soup = bs(driver.page_source, 'html.parser') # 3초가 끝나면 새로운 page의 url로 접속하고 파서가 파싱한다.

        # 현 page에서 petition 요소에 접근
        lists = soup.find("div", attrs={"class": "ct_list1"})
        ul = lists.find("ul", attrs={"class": "petition_list"})
        a_lists = ul.find_all("a", attrs={"class": "cb relpy_w"})  # 현재 페이지의 "전체목록" 내부에 있는 모든 a 태그들을 리스트로 반환.

        # path를 추가하기 전에 현재 paths 리스트에 몇개가 들어있는지 확인한다 => 2 page가 되면 paths 리스트의 7번 index부터 읽어야하기 때문
        path_len = len(paths)
        for a in a_lists: # 현재 page 내에서 각 petition의 title과 path를 파싱하여 paths, titles 배열에 저장.
            # print(a.attrs["href"]) path 배열에 들어갈 경로 "/petition/12345"
            paths.append(a.attrs["href"])  # 각 청원의 path를 저장.
            # print(a.text[3:].strip()) titles
            titles.append(a.text[3:].strip())  # 각 청원의 title을 titles 배열에 저장.

        new_paths = paths[path_len:] # page 별로 해당하는 path만 읽어야하므로 => 2 page가 되면 paths 리스트의 7번 index부터 읽어야하기 때문
        print("new_paths : ", new_paths)
        for path in new_paths: # 현재 page의 모든 petition들의 해당 url에 접속하여 content를 파싱.
            url = "https://www1.president.go.kr" + path
            driver.get(url)
            soup = bs(driver.page_source, 'html.parser') # 각 url에 대하여 soup 객체를 새로 생성한다.

            # contents 추가하기.
            content = soup.find("div", attrs={"class":"View_write"})
            data = content.text.strip()
            data = data.replace("\n", "") # \n 문자도 제거
            print('본문 내용 : ', data)

            # date 추가하기.
            date_ul = soup.find("ul", attrs={"class": "petitionsView_info_list"})
            li = date_ul.li.next_sibling.next_sibling
            li_data = li.text[4:]


            # num 추가하기.
            h2 = soup.find("h2", attrs={"class": "petitionsView_count"})
            num = h2.find('span', attrs={"class": "counter"})
            num = num.text

            contents.append(data) # 공백 제거한 data를 리스트에 저장.
            dates.append(li_data)
            nums.append(num)
    driver.close()
    # 최종 값
    print("titles : ", titles)
    print("nums 배열의 수로 확인하기 ", len(nums)) # num의 갯수가 700개(100페이지 * 7개 청원글)여야 안뺴놓고 크롤링 한거임.
    print("contents : ", contents)
    print("dates : ", dates)
    print("nums : ", nums)

    my_dictionary = {"title": titles, "date": dates, "num": nums, "contents": contents}
    data = pd.DataFrame(my_dictionary)  # 전체 데이터를 긁은 list인 total을 dataframe으로 변환시켜주면서 각 column의 이름을 부여해줍니다.

    str_i = str(i)
    str_i2 = str(i+99)
    datatitle = str_i + '_' + str_i2
    # datatitle = input("Please set the title of this excel file : ")  # 긁어온 데이터를 저장할 xlsx 파일의 이름을 지정해줍니다.
    data.to_excel("crawling_data/" + datatitle + ".xlsx", engine="xlsxwriter")
    #xlsxwriter를 임포트시켜서 engine을 추가하지 않으면, data.to_excel에서 IllegalCharacterError 에러가 뜬다.

