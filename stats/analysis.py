# Importing necessary libraries
import pandas as pd
import numpy as np
import os
from node import *

ALGS = ['flood', 'random_walk', 'normalized_flood']
TYPES = ['all', 'disabled']
stat_folder = 'test_results/'


def main():
    directories = os.listdir(os.getcwd())

    # for disabled and not disabled types
    for network_type in directories:
        if os.path.isdir(network_type) and network_type in TYPES:
            print(f"Directory: {network_type}")
            topologies = os.listdir(network_type)

            # for each topology
            for topology in topologies:
                print(f"    Topology: {topology}")
                topology_path = os.path.join(network_type, topology)
                algs = os.listdir(topology_path)
                # for each algorithm
                for alg in algs:

                    tests: list[Test] = []
                    if alg in ALGS:
                        print(f"      Algorithm: {alg}")
                        cur_path = os.path.join(topology_path, alg)
                        files = os.listdir(cur_path)
                        pairs = find_pair_files(files)

                        # for each pair of files
                        for pair in pairs:
                            print(f"        Pair: {pair}")
                            # read the files
                            nodes = read_file_pair(cur_path, pair)

                        #     # run the analysis
                        #     test: Test = analyze_nodes(nodes)
                        #     tests.append(test)

                        #     # write individual test to a file just in case
                        #     write_test_to_file(
                        #         test, os.path.join(d, stat_folder), pair[0])

                        # # average the tests
                        # combined_test = AllTestsAveraged(
                        #     tests).calculate_averages()
                        # # write the tests
                        # write_test_to_file(
                        #     combined_test, f"{d}", f"main_test_{alg}")
                            break  # temp
                    break  # temp
                break  # temp

            break  # temp


# def analyze_nodes(nodes: list[SrcNode, SinkNode, list[IntermediateNode]]) -> Test:
#     intermediate_nodes: list[IntermediateNode] = nodes[2]
#     intermediate_node_tests = analyze_intermediate_nodes(intermediate_nodes)

#     src_node: SrcNode = nodes[0]
#     src_node_tests, query_hit_tests = analyze_src_node(src_node)

#     sink_node: SinkNode = nodes[1]
#     sink_node_tests = analyze_sink_node(sink_node)

#     return Test(src_node_tests, sink_node_tests, intermediate_node_tests, query_hit_tests)


# def analyze_src_node(srcNode: SrcNode) -> Test.SrcNode_Per_Test_Average:
#     # analyze query hits
#     avg_hops, avg_latency = srcNode.analyze_query_hits()

#     return (
#         Test.SrcNode_Per_Test_Average(
#             srcNode.initial_requests, srcNode.tried_requests, srcNode.query_hits
#         ),
#         Test.QueryHit_Per_Test_Average(avg_hops, avg_latency)
#     )


# # this will be useful for dynamic nodes
# def analyze_sink_node(sinkNode: SinkNode) -> Test.SinkNode_Per_Test_Average:
#     return Test.SinkNode_Per_Test_Average(
#         sinkNode.received_requests
#     )


# def analyze_intermediate_nodes(intermediateNodes: list[IntermediateNode]) -> Test.IntermediateNode_Per_Test_Average:
#     n = len(intermediateNodes)
#     sum_sent = 0
#     sum_received = 0
#     sum_forwarded = 0
#     sum_wasted_requests = 0
#     sum_efficiency = 0
#     for node in intermediateNodes:
#         sum_sent += node.sent_requests
#         sum_received += node.received_requests
#         sum_forwarded += node.forwarded_hits
#         sum_wasted_requests += node.wasted_requests()
#         sum_efficiency += node.efficiency()

#     avg_sent = round(sum_sent / n, 1)
#     avg_received = round(sum_received / n, 1)
#     avg_forwarded = round(sum_forwarded / n, 1)
#     avg_wasted_requests = round(sum_wasted_requests / n, 1)
#     avg_efficiency = round(sum_efficiency / n, 1)

#     return Test.IntermediateNode_Per_Test_Average(
#         avg_wasted_requests,
#         avg_efficiency,
#         avg_sent,
#         avg_received,
#         avg_forwarded
#     )


def read_file_pair(path, pair) -> list[SrcNode, SinkNode, list[IntermediateNode]]:
    # get file
    main_file, query_hits_file = pair
    main_file = os.path.join(path, main_file)
    query_hits_file = os.path.join(path, query_hits_file)

    # read the files
    main_df = pd.read_csv(main_file)
    read_main_df(main_df)
    # query_hits_df = pd.read_csv(query_hits_file)

    # # get df data
    # nodes = read_main_df(main_df)
    # queryHits = read_query_hits_df(query_hits_df)

    # nodes[0].setQueryHitList(queryHits)

    # return nodes


def read_main_df(main_df: pd.DataFrame):
    # sort nodes
    src_node = main_df[main_df['IsSource'] == 1].iloc[0].copy()
    sink_node = main_df[main_df['IsSink'] == 1].iloc[0].copy()
    i_nodes = main_df[(
        main_df['IsSource'] == 0) & (main_df['IsSink'] == 0)].copy()

    i_nodes = calculate_wasted_requests(i_nodes)

    print(src_node.describe())
    print("--------------")
    print(sink_node.describe())
    print("--------------")
    print(i_nodes.describe())

    mean_received_req = i_nodes['ReceivedRequests'].mean()
    mean_sent_req = i_nodes['SentRequests'].mean()
    mean_fquery_hits = i_nodes['ForwardedQueryHits'].mean()
    mean_wasted_requests = i_nodes['WastedRequests'].mean()

    print(f"Mean Received Requests: {mean_received_req}")
    print(f"Mean Sent Requests: {mean_sent_req}")
    print(f"Mean Forwarded Query Hits: {mean_fquery_hits}")
    print(f"Mean Wasted Requests: {mean_wasted_requests}")
    # srcNode = SrcNode(src_node['NodeID'], src_node['TriedRequests'],
    #                   src_node['InitializedRequests'], src_node['QueryHits'])
    # sinkNode = SinkNode(sink_node['NodeID'], sink_node['ReceivedRequests'])
    # intermediateNodes = []
    # for _, row in intermediate_nodes.iterrows():
    #     intermediateNodes.append(IntermediateNode(
    #         row['NodeID'], row['ReceivedRequests'], row['SentRequests'], row['ForwardedQueryHits']))

    # return [srcNode, sinkNode, intermediateNodes]


def read_query_hits_df(query_hits_df: pd.DataFrame) -> list[QueryHit]:
    queryHits = []
    for _, row in query_hits_df.iterrows():
        queryHits.append(
            QueryHit(row['QueryHitId'], row[' Hops'], row[' Seconds']))

    return queryHits


def find_pair_files(files) -> list[tuple[str, str]]:
    pairs = []

    for file in files:
        # remove the extension
        base = file.split('.')[0]

    # ccheck if it's a main file or query hits file
    if '_queryhits_' in base:
        # found query hit then main file is the same name
        main_file = base.replace('_queryhits_', '_') + '.csv'
    if main_file in files:
        pairs.append((main_file, file))

        return pairs


def calculate_wasted_requests(df):
    # Vectorized calculation with proper zero-division handling
    mask = df['ReceivedRequests'] > 0
    df['WastedRequests'] = 0  # Default value
    df.loc[mask, 'WastedRequests'] = (
        (df['SentRequests'] - df['ForwardedQueryHits']) /
        df['ReceivedRequests']
    )[mask]
    return df


def write_test_to_file(test: Test, cur_path: str, filename: str):
    # write the test to a local file
    with open(f'{cur_path}/{filename}.txt', 'w') as f:
        f.write(str(test))


main()
