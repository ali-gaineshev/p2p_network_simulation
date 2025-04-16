# Importing necessary libraries
import pandas as pd
import numpy as np
import os
from node import *

ALGS = ['flood', 'random_walk', 'normalized_flood']
TOPOLOGIES = ['3_regular_10_nodes', '3_regular_20_nodes', '3_regular_30_nodes',
              '3_regular_40_nodes', '3_regular_50_nodes', '4_regular_200_nodes',
              '5_regular_200_nodes', 'cluster_6_with_4_nodes_each', 'cluster_6_with_10_nodes_each',
              'cluster_6_with_20_nodes_each', 'cluster_6_with_30_nodes_each', 'megagraph_3_clusters',
              'megagraph_5_clusters', 'tree_with_10_nodes', 'tree_with_20_nodes', 'tree_with_30_nodes',
              'tree_with_40_nodes', 'tree_with_50_nodes', 'tree_with_60_nodes', 'tree_with_70_nodes',
              'tree_with_80_nodes']
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
                if topology not in TOPOLOGIES:
                    continue

                print(f"    Topology: {topology}")
                topology_path = os.path.join(network_type, topology)
                algs = os.listdir(topology_path)
                # for each algorithm
                for alg in algs:
                    if alg not in ALGS:
                        continue

                    tests: list[Test] = []
                    print(f"      Algorithm: {alg}")
                    cur_path = os.path.join(topology_path, alg)
                    files = os.listdir(cur_path)
                    pairs = find_pair_files(files)

                    # test object for each pair
                    int_node_tests = IntermediateNodeTests()
                    src_node_tests = SrcNodeTests()
                    avg_query_hits_tests = AvgQueryHitsTests()
                    raw_query_hits_tests = RawQueryHitsTests()

                    # for each pair of files
                    count = 0
                    for pair in pairs:
                        # print(f"        Pair: {pair}")
                        # read the files and get data
                        src_node, int_nodes, query_hits_df, per_avg_qh_stats = read_file_pair(
                            cur_path, pair
                        )

                        int_node_tests.append_row(int_nodes)
                        src_node_tests.append_row(src_node)
                        avg_query_hits_tests.append_row(per_avg_qh_stats)
                        raw_query_hits_tests.append_row(query_hits_df)
                        count += 1
                    print(f"        Count: {count}")
                    # calculate combined stats
                    local_test_for_int_nodes = int_node_tests.calculate_stats()
                    local_test_for_src_nodes = src_node_tests.calculate_stats()
                    local_test_for_avg_qh = avg_query_hits_tests.calculate_stats()
                    local_test_for_raw_qh = raw_query_hits_tests.calculate_stats()

                    test = Test(local_test_for_src_nodes, local_test_for_avg_qh,
                                local_test_for_raw_qh, local_test_for_int_nodes)
                    test.write_to_txt_file(
                        topology_path, f"{alg}_test_results.md", alg)


# READING MAIN CSV FILES AND CONVERTING THEM TO DATAFRAMES


def read_file_pair(path, pair):
    # get file
    main_file, query_hits_file = pair
    main_file = os.path.join(path, main_file)
    query_hits_file = os.path.join(path, query_hits_file)

    # read main csv files
    main_df = pd.read_csv(main_file)
    src_node, int_nodes = read_main_df(main_df)
    # read query hits csv files
    query_hits_df = pd.read_csv(query_hits_file)
    per_avg_qh_stats, query_hits_df = read_query_hits_df(query_hits_df)

    return src_node, int_nodes, query_hits_df, per_avg_qh_stats


def read_main_df(main_df: pd.DataFrame):

    # intermediate nodes
    i_nodes = main_df[(
        main_df['IsSource'] == 0) & (main_df['IsSink'] == 0)].copy()
    # calculate new columns
    i_nodes = calculate_wasted_requests(i_nodes)
    i_nodes = calculate_efficiency(i_nodes)
    i_nodes = calculate_zero_work_done_in_nodes(i_nodes)

    # Compute summary statistics
    int_nodes_summary_data = {
        'TotalNodes': [len(i_nodes)],
        'MeanReceivedRequests': [i_nodes['ReceivedRequests'].mean()],
        'MeanSentRequests': [i_nodes['SentRequests'].mean()],
        'MeanForwardedQueryHits': [i_nodes['ForwardedQueryHits'].mean()],
        'MeanWastedRequests': [i_nodes['WastedRequests'].mean()],
        'MeanQueryHitEfficiency': [i_nodes['Efficiency'].mean()],
        'CountDisabledNodes': [i_nodes['IsDisabled'].sum()],
        'CountZeroWorkDone': [i_nodes['NoWorkDone'].sum() - i_nodes['IsDisabled'].sum()],
        'TotalMessages': [i_nodes['SentRequests'].sum() + i_nodes['ForwardedQueryHits'].sum() + i_nodes['ReceivedRequests'].sum()],
    }
    int_nodes_summary_df = pd.DataFrame(int_nodes_summary_data)

    int_nodes_total_messages = int_nodes_summary_df['TotalMessages'][0]
    # src node
    src_node = main_df[main_df['IsSource'] == 1].iloc[0].copy().to_frame().T
    src_node = src_node.drop(columns=[
        'NodeID', 'IsSink', 'IsSource', 'IsDisabled', 'SentRequests', 'ForwardedQueryHits', 'ReceivedRequests'
    ])
    src_node['SuccessRate'] = (
        src_node['UniqueQueryHits'] / src_node['InitializedRequests']) * 100
    src_node['RedundantQueryHits'] = src_node.apply(
        lambda row: 0 if row['UniqueQueryHits'] == 0 else (
            (row['QueryHits'] - row['UniqueQueryHits']) / row['QueryHits']) * 100,
        axis=1
    )
    src_node = src_node.rename(columns={'TriedRequests': 'TotalRetries'})
    if src_node['UniqueQueryHits'].iloc[0] > 0:
        src_node['OverheadPerQueryHit'] = int_nodes_total_messages / \
            src_node['UniqueQueryHits'].iloc[0]
    else:
        src_node['OverheadPerQueryHit'] = float('nan')

    return src_node, int_nodes_summary_df


def read_query_hits_df(query_hits_df: pd.DataFrame):
    per_average_stats = pd.DataFrame({
        'AvgHops': [query_hits_df['Hops'].mean()],
        'AvgLatency': [query_hits_df['Seconds'].mean()],
    })
    # keep raw data to combine with others later
    query_hits_df.drop(columns=['QueryHitId'], inplace=True)

    return per_average_stats, query_hits_df

# READING AND WRITING FUNCTIONS


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


# UTILITY FUNCTIONS


def calculate_wasted_requests(df):
    mask = df['ReceivedRequests'] > 0
    df['WastedRequests'] = 0.0  # Default value

    # calc the value only when ReceivedRequests > 0 AND ForwardedQueryHits <= SentRequests
    calc_mask = mask & (df['ForwardedQueryHits'] <= df['SentRequests'])
    df.loc[calc_mask, 'WastedRequests'] = (
        ((df['SentRequests'] - df['ForwardedQueryHits']) /
         df['ReceivedRequests']) * 100
    )[calc_mask]

    # set to 0 otherwise
    df.loc[mask & (df['ForwardedQueryHits'] > df['SentRequests']),
           'WastedRequests'] = 0

    return df


def calculate_efficiency(df):
    mask = df['SentRequests'] > 0
    df['Efficiency'] = 0.0  # Default value
    df.loc[mask, 'Efficiency'] = (
        (df['ForwardedQueryHits'] / df['SentRequests']) * 100
    )[mask]

    return df


def calculate_zero_work_done_in_nodes(df):
    df['NoWorkDone'] = (
        (df['ReceivedRequests'] == 0) &
        (df['SentRequests'] == 0) &
        (df['ForwardedQueryHits'] == 0)
    )

    return df


main()
