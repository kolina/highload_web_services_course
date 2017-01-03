import json
import random
import uuid

import numpy

from collections import defaultdict

from pymongo import MongoClient

requests = ["get_message", "create_message"]
plan = {"count": 100000, "probs": [0.6, 0.4]}
create_new_thread_prob = 0.2

def main():
    client = MongoClient("mongodb://95.108.186.240:27017,95.108.186.242:27017,95.108.186.217:27017/?replicaSet=rs0&w=majority&readConcernLevel=linearizable&readPreference=primary")
    coll = client["mail_service"]["mails"]


    threads = defaultdict(list)
    mails = list()
    thread_ids = list()
    for mail in coll.find():
        threads[mail["thread_id"]].append(mail)
        mails.append(mail)
        thread_ids.append(mail["thread_id"])

    threads = {thread_id: sorted(mails, key=lambda k: k["created_at"]) for thread_id, mails in threads.iteritems()}
    
    for _ in xrange(plan["count"]):
        request = numpy.random.choice(numpy.array(requests), 1, p=plan["probs"])

        if request == "get_message":
            mail = random.choice(mails)
            user = random.choice([mail["from"]] + mail["to"])
            print "{}||{}||{}".format('GET', '/{}/messages/{}'.format(user, mail["_id"]), 'get_message')
        else:
            if random.random() <= create_new_thread_prob:
                thread_id = str(uuid.uuid4())
                thread_ids.append(thread_id)
                threads[thread_id] = list()
                users_count = random.randint(2, 10)
                users = map(str, range(1, 11))
                random.shuffle(users)
                users = users[:users_count]
                previous_user = "0"
            else:
                thread_id = random.choice(thread_ids)
                mail = threads[thread_id][-1]
                users = [mail["from"]] + mail["to"]
                previous_user = mail["from"]
            message_id = str(uuid.uuid4())
            current_from = random.choice(filter(lambda x: x != previous_user, users))
            to = filter(lambda x: x != current_from, users)
            mail = {"_id": message_id, "thread_id": thread_id, "from": current_from, "to": to}
            threads[thread_id].append(mail)
            mails.append(mail)
            print "{}||{}||{}||{}".format('PUT', '/{}/messages?threadId={}'.format(current_from, thread_id), 'create_message', json.dumps({"to": to})) 


if __name__ == '__main__':
    main()
