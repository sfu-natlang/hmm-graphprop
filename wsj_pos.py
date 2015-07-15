#from hmmpy_multi import HMM
#from hmmpy_multi import baum_welch
#from hmmpy_multi import viterbi

from hmmpy_multi_graph import HMM
from hmmpy_multi_graph import baum_welch
from hmmpy_multi_graph import viterbi

import numpy as np
import cPickle as pickle
import operator

# smart random start
#closeness_factor=100. # more is more close results
#start_param = np.random.dirichlet(np.ones(len(states))*closeness_factor,size=1)[0]    
#transition  = []
#for i in xrange(len(states)):
#    #print np.random.dirichlet(np.ones(len(states))*closeness_factor,size=1)[0]
#    transition.append(np.random.dirichlet(np.ones(len(states))*closeness_factor,size=1)[0])

def save_object(obj, filename):
    with open(filename, 'wb') as output:
        pickle.dump(obj, output, pickle.HIGHEST_PROTOCOL)

def load_object(filename):
    with open(filename, 'rb') as input:
        return pickle.load(input)



# test types : many-to-one, one-to-one
def compute_error(hmm,dev_file,test_file,test_type='many_to_one'):
    total = 0
    correct = 0
    Co_occurace_dic = {}
    Lable_occurance = {}
    Real_lable_occurance = {}
    with open(dev_file) as inp:
        seq_list = []
        real_label_list = []
        temp_list = []
        number_of_tags=0
        for line in inp:
            if line:
                seq_list.append(line.split()[0])
                #if "|" in line.split()[1]:
                #    real_label_list.append(line.split()[1].split('|')[0])
                #else:
                #    real_label_list.append(line.split()[1])

                real_label_list.append(line.split()[1])

    # running viterbi
            if line.split()[0] == ".":
                #print seq_list 
                label_list,_t,_t2 = viterbi(hmm, seq_list, scaling=True )
                #print label_list,_t,_t2
                #mapping = ["NN","IN","NNP","DT","NNS","JJ",",",".","CD","VBD","RB","VB","CC","TO","VBN","VBZ","PRP","VBG","VBP","MD","POS","PRP$","$","``","''",":","WDT","JJR","NNPS","WP","WRB","JJS",")","(","RBR","RP","EX","PDT","RBS","#","FW","WP$","UH","SYM","LS"]

    # sort by freq and top words (heuristic)    
    # TODO for all permutation 
                assert len(label_list)==len(real_label_list)
                for t in xrange(len(label_list)):
                    number_of_tags += 1
                    #print real_label_list[t] ,label_list[t]
                #loc = int(str(label_list[t])[1:])
                    if "|" in real_label_list[t]:

                        if label_list[t] in Lable_occurance:
                            Lable_occurance[label_list[t]] = Lable_occurance[label_list[t]] + 1
                        else:
                            Lable_occurance[label_list[t]] = 1


                        for item in real_label_list[t].split('|'):
                            if item in Real_lable_occurance:
                                Real_lable_occurance[item] = Real_lable_occurance[item] + 1
                            else:
                                Real_lable_occurance[item] = 1

                            Co_occurance_string= str(label_list[t]) +" ||| "+ item
                            if Co_occurance_string in Co_occurace_dic:
                                Co_occurace_dic[Co_occurance_string] =  Co_occurace_dic[Co_occurance_string] + 1
                            else:
                                Co_occurace_dic[Co_occurance_string] = 1
                    else:

                        if label_list[t] in Lable_occurance:
                            Lable_occurance[label_list[t]] = Lable_occurance[label_list[t]] + 1
                        else:
                            Lable_occurance[label_list[t]] = 1 
                        if real_label_list[t] in Real_lable_occurance:
                            Real_lable_occurance[real_label_list[t]] = Real_lable_occurance[real_label_list[t]] + 1
                        else:
                            Real_lable_occurance[real_label_list[t]] = 1

                        Co_occurance_string= str(label_list[t]) +" ||| "+ real_label_list[t]
                        if Co_occurance_string in Co_occurace_dic:
                            Co_occurace_dic[Co_occurance_string] =  Co_occurace_dic[Co_occurance_string] + 1
                        else:
                            Co_occurace_dic[Co_occurance_string] = 1

                
                    #if (real_label_list[t]== mapping[label_list[t]]):
                    #    correct += 1
                    #if seq_list[t] == "will":
                    #   temp_list.append(mapping[label_list[t]])
                seq_list = []
                real_label_list = []
    print sorted(Co_occurace_dic.items(), key=operator.itemgetter(1),reverse=True)
    
    mapping = {}
    processed = []

    for (item,value2) in sorted(Co_occurace_dic.items(), key=operator.itemgetter(1),reverse=True):
        key = item.split(" ||| ")[0]
        value = item.split(" ||| ")[1]
        #print key, value2
        if not key in processed:
            mapping[key] = value
            processed.append(key)

    print mapping
    correct = 0
    total = 0
    with open(test_file) as inp:
        seq_list = []
        real_label_list = []
        for line in inp:
            if line:
                seq_list.append(line.split()[0])
                #if "|" in line.split()[1]:
                #    real_label_list.append(line.split()[1].split('|')[0])
                #else:
                #    real_label_list.append(line.split()[1])
                real_label_list.append(line.split()[1])

    # running viterbi
            if line.split()[0] == ".":
                label_list,_t,_t2 = viterbi(hmm, seq_list, scaling=True )

    # sort by freq and top words (heuristic)    
    # TODO for all permutation 
                assert len(label_list)==len(real_label_list)
                for t in xrange(len(label_list)):
                    total += 1
                    if str(label_list[t]) in mapping:
                        if "|" in real_label_list[t]:
                            print real_label_list[t].split('|')
                            for item in real_label_list[t].split('|'):
                                if (item == mapping[str(label_list[t])]):
                                    correct += 1
                        else:
                            if (real_label_list[t]== mapping[str(label_list[t])]):
                                correct += 1
                seq_list = []
                real_label_list = []

    print float(correct) / float(total)


def hmm_train(hmm, training_data_file,number_of_epochs, step_size_of_writing, unique_name,number_of_processors,name_of_file_for_storing):

    with open(training_data_file) as inp:
        label_list  = []
        seq_list = []
        train_seq= []
        for line in inp:
            if line:
                seq_list.append(line.split()[0])
                label_list.append(line.split()[1])
                if line.split()[0] == ".":
                    train_seq.append(seq_list)
                    label_list  = []
                    seq_list = []
    # TODO add validation set
    internal_iter = number_of_epochs/step_size_of_writing 
    for it in xrange(internal_iter):
        baum_welch(hmm,train_seq,epochs=step_size_of_writing,numberOfProcesses=number_of_processors, verbose=True, graph=False, graphprop=1)
        if name_of_file_for_storing != "":
            save_object(hmm,unique_name+str((it+1)*step_size_of_writing)+".pkl")


# Data location
#training_data_file = "/cs/natlang-user/ramtin/git/HMM/test_tasks/pos_tagging/test/wsj_00-18.list"
#dev_data_file = "/cs/natlang-user/ramtin/git/HMM/test_tasks/pos_tagging/test/wsj_02-04.list"
#test_data_file = "/cs/natlang-user/ramtin/git/HMM/test_tasks/pos_tagging/test/wsj_02-04.list"


training_data_file =  "/cs/natlang-user/ramtin/git/hmm_graph/tasks/pos/train/wsj_all.list"
#dev_data_file =  "/cs/natlang-user/ramtin/git/hmm_graph/tasks/pos/train/wsj_all.list"
#test_data_file =  "/cs/natlang-user/ramtin/git/hmm_graph/tasks/pos/train/wsj_all.list"

dev_data_file = "/cs/natlang-user/ramtin/git/hmm_graph/tasks/pos/test/wsj_19-21.list"
test_data_file = "/cs/natlang-user/ramtin/git/hmm_graph/tasks/pos/test/wsj_22-24.list"

vocab_file = "tasks/pos/train/wsj_vocab_list"
# Operation type
train_opr = True
test_opr = True
print_mapping = False

# parameters
number_of_hidden_variables = 45
number_of_processors= 40
number_of_epochs= 40
step_size_of_writing = 5
unique_name = "models/wsj_pos_45class_rand222_graph_hmm_newprobablities_itr20+"
name_of_file_for_loading = "models/wsj_pos_45class_rand222_graph_hmm_newprobablities_itr20.pkl" #models/wsj_pos_45class_rand3_hmm_itr50.pkl"#models/wsj_pos_45class_rand1_hmm_itr50_sentBysent.pkl" #models/wsj_pos_45class_rand1_hmm_itr45_sentBysent.pkl"
name_of_file_for_storing = "temp"#models/wsj_pos_45class_rand1_hmm_itr60_sentBysent.pkl"


if name_of_file_for_loading == "":
    vocab_list = []
    with open(vocab_file) as inp:
        for line in inp:
            vocab_list.append(line.strip())

    # <><><><><><><><><><><><><><><>

    # smart random start
    closeness_factor=1000. # more is more close results
    #start_param = np.random.dirichlet(np.ones(len(states))*closeness_factor,size=1)[0]
    transition  = []

    number_of_states = 45
    for i in xrange(number_of_states):
    #print np.random.dirichlet(np.ones(len(states))*closeness_factor,size=1)[0]
        transition.append(np.random.dirichlet(np.ones(number_of_states)*closeness_factor,size=1)[0])

    print "transition probs set"
    emission = []
    for i in xrange(number_of_states):
        emission.append(np.random.dirichlet(np.ones(len(vocab_list))*closeness_factor,size=1)[0])


    #new_hmm = HMM(number_of_hidden_variables,A=np.array(transition), B=np.array(emission) ,V=np.array(vocab_list))
    # <><><><><><><><><><><><><><><>
    #new_hmm = HMM(number_of_hidden_variables,V=np.array(vocab_list))

    # Transition uniform but emission not
    new_hmm = HMM(number_of_hidden_variables,A=np.array(transition),V=np.array(vocab_list))

else:
    new_hmm = load_object(name_of_file_for_loading)

if train_opr == True:
    hmm_train(new_hmm,training_data_file,number_of_epochs, step_size_of_writing, unique_name,number_of_processors,name_of_file_for_storing)

if test_opr == True:
    compute_error(new_hmm,dev_data_file,test_data_file)


