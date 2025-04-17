import pandas as pd


class Test:
    def __init__(self, topology, algorithm, network_type, local_test_for_src_nodes, local_test_for_avg_qh, local_test_for_raw_qh, local_test_for_int_nodes):
        self.topology = topology
        self.algorithm = algorithm
        self.network_type = network_type
        self.test_for_int_nodes: Test.Test_For_IntermediateNodes = local_test_for_int_nodes
        self.test_for_src_nodes: Test.Test_For_SourceNodes = local_test_for_src_nodes
        self.test_for_avg_qh: Test.Test_For_AvgQueryHits = local_test_for_avg_qh
        self.test_for_raw_qh: Test.Test_For_RawQueryHits = local_test_for_raw_qh

    def get_table_row(self) -> list[str]:
        def format_scientific_superscript(value: float) -> str:
            # Format in scientific notation like 1.23e-05
            base, exponent = f"{value:.2e}".split("e")
            exp = int(exponent)
            superscript_map = str.maketrans("-0123456789", "⁻⁰¹²³⁴⁵⁶⁷⁸⁹")
            return f"{base}×10{str(exp).translate(superscript_map)}"

        return [
            f"{self.topology}-{self.network_type}",
            self.algorithm,
            f"{self.test_for_src_nodes.avg_total_retries:.1f}",
            f"{self.test_for_src_nodes.avg_unique_qh:.1f}",
            f"{int(self.test_for_src_nodes.true_avg_success_rate)}%",
            f"{int(self.test_for_src_nodes.avg_redundant_qh)}%",
            f"{self.test_for_avg_qh.avg_hops:.1f}",
            format_scientific_superscript(self.test_for_avg_qh.avg_latency),
            f"{self.test_for_int_nodes.avg_total_work:.1f}",
            f"{int(self.test_for_int_nodes.avg_wasted_req_perc)}%",
            f"{int(self.test_for_int_nodes.avg_qh_efficiency)}%",
            f"{self.test_for_int_nodes.avg_zero_work_done_nodes:.1f}",
        ]

    def combine_qh_tests_to_str(self) -> str:
        avg_latency = f"{self.test_for_avg_qh.avg_latency:.2e}".replace(
            "e", " × 10<sup>") + f"</sup>"
        min_latency = f"{self.test_for_raw_qh.min_latency:.2e}".replace(
            "e", " × 10<sup>") + f"</sup>"
        max_latency = f"{self.test_for_raw_qh.max_latency:.2e}".replace(
            "e", " × 10<sup>") + f"</sup>"
        median_latency = f"{self.test_for_raw_qh.median_latency:.2e}".replace(
            "e", " × 10<sup>") + f"</sup>"
        std_latency = f"{self.test_for_raw_qh.std_dev_latency:.2e}".replace(
            "e", " × 10<sup>") + f"</sup>"
        return (
            f'<pre><code>'
            f"Average Hops: {self.test_for_avg_qh.avg_hops:.2f}\n\n"
            f"Min Hops: {self.test_for_raw_qh.min_hops:.2f}\n\n"
            f"Max Hops: {self.test_for_raw_qh.max_hops:.2f}\n\n"
            f"Median Hops: {self.test_for_raw_qh.median_hops:.2f}\n\n"
            f"Standard Deviation of Hops: {self.test_for_raw_qh.std_dev_hops:.2f}\n\n\n"

            f"Average Latency: {avg_latency}\n\n"
            f"Max Latency: {max_latency}\n\n"
            f"Min Latency: {min_latency}\n\n"
            f"Median Latency: {median_latency}\n\n"
            f"Standard Deviation of Latency: {std_latency}\n\n"
            f'</code></pre>\n'
        )

    def write_to_txt_file(self, path, filename, alg):
        with open(f"{path}/{filename}", "w") as file:
            file.write(f"# {path} --- {alg}\n")
            file.write("## Source Nodes Test Results:\n")
            file.write(str(self.test_for_src_nodes))
            file.write("\n\n## Average Query Hits Test Results:\n")
            file.write(str(self.combine_qh_tests_to_str()))
            file.write("\n---------------------------------------------\n")
            file.write("## Intermediate Nodes Test Results:\n\n")
            file.write(str(self.test_for_int_nodes))

    class Test_For_IntermediateNodes:
        def __init__(self, avg_received_req, avg_sent_req, avg_f_qh, avg_wasted_req_perc,
                     avg_qh_efficiency, avg_disabled_nodes, avg_zero_work_done_nodes,
                     avg_total_work):
            self.avg_received_req = avg_received_req
            self.avg_sent_req = avg_sent_req
            self.avg_f_qh = avg_f_qh
            self.avg_wasted_req_perc = avg_wasted_req_perc
            self.avg_qh_efficiency = avg_qh_efficiency
            self.avg_disabled_nodes = avg_disabled_nodes
            self.avg_zero_work_done_nodes = avg_zero_work_done_nodes
            self.avg_total_work = avg_total_work

        def __str__(self):
            return (
                f"\tAverage Total Work (Sent + Received + Forwarded QH): {int(self.avg_total_work)}\n\n"
                f"\tAverage Wasted Requests ((Sent - Forwarded QH) / Received): {int(self.avg_wasted_req_perc)}%\n\n"
                f"\tAverage Query Hit Efficiency (Forwarded QH / Received): {int(self.avg_qh_efficiency)}%\n\n"
                f"\tAverage Count of Zero Work Done Nodes (No Sent, Received, Forwarded QH): {self.avg_zero_work_done_nodes:.2f}\n\n"
                f"\tAverage Sent Requests: {self.avg_sent_req:.2f}\n\n"
                f"\tAverage Forwarded Query Hits: {self.avg_f_qh:.2f}\n\n"
                f"\tAverage Received Requests: {self.avg_received_req:.2f}\n\n"
                f"\tAverage Count of Disabled Nodes: {self.avg_disabled_nodes:.2f}\n\n"
            )

    class Test_For_SourceNodes:
        def __init__(self, avg_qh, median_qh,
                     max_qh, avg_unique_qh, avg_total_retries,
                     avg_initialized_req, true_avg_success_rate, avg_success_rate, avg_redundant_qh,
                     avg_overhead_per_qh):
            self.avg_qh = avg_qh
            self.avg_unique_qh = avg_unique_qh
            self.median_qh = median_qh
            self.max_qh = max_qh
            self.avg_total_retries = avg_total_retries
            self.avg_initialized_req = avg_initialized_req
            self.true_avg_success_rate = true_avg_success_rate
            self.avg_success_rate = avg_success_rate
            self.avg_redundant_qh = avg_redundant_qh
            self.avg_overhead_per_qh = avg_overhead_per_qh

        def __str__(self):
            return (
                f"\tAverage Query Hits: {self.avg_qh:.2f}\n\n"
                f"\tMedian Query Hits: {self.median_qh:.2f}\n\n"
                f"\tMax Query Hits: {self.max_qh:.2f}\n\n"
                f"\tAverage Unique Query Hits: {self.avg_unique_qh:.2f}\n\n"
                f"\tAverage Total Retries: {self.avg_total_retries:.2f}\n\n"
                f"\tAverage Initialized Requests: {self.avg_initialized_req:.2f}\n\n"
                f"\tAverage True Success Rate (Any Query Hits): {int(self.true_avg_success_rate)} %\n\n"
                f"\tAverage Success Rate (Unique Query Hits / Initialized Requests): {int(self.avg_success_rate)} %\n\n"
                f"\tAverage Redundant Query Hits (Unique Query Hits - Query Hits): {int(self.avg_redundant_qh)} %\n\n"
                f"\tAverage Overhead Per Query Hit (Total Messages / Unique Query Hit): {int(self.avg_overhead_per_qh)}\n\n"
            )

    class Test_For_AvgQueryHits:
        def __init__(self, avg_hops, avg_latency):
            self.avg_hops = avg_hops
            self.avg_latency = avg_latency

        def __str__(self):
            return (
                f"Average Hops: {self.avg_hops}\n"
                f"Average Latency: {self.avg_latency}"
            )

    class Test_For_RawQueryHits:
        def __init__(self, avg_hops, min_hops, max_hops, median_hops, std_dev_hops,
                     avg_latency, min_latency, max_latency, median_latency, std_dev_latency):
            self.avg_hops = avg_hops
            self.min_hops = min_hops
            self.max_hops = max_hops
            self.median_hops = median_hops
            self.std_dev_hops = std_dev_hops
            self.avg_latency = avg_latency
            self.min_latency = min_latency
            self.max_latency = max_latency
            self.median_latency = median_latency
            self.std_dev_latency = std_dev_latency

        def __str__(self):
            return (
                f"Average Hops: {self.avg_hops}\n"
                f"Min Hops: {self.min_hops}\n"
                f"Max Hops: {self.max_hops}\n"
                f"Median Hops: {self.median_hops}\n"
                f"Standard Deviation of Hops: {self.std_dev_hops}\n"
                f"Average Latency: {self.avg_latency}\n"
                f"Min Latency: {self.min_latency}\n"
                f"Max Latency: {self.max_latency}\n"
                f"Median Latency: {self.median_latency}\n"
                f"Standard Deviation of Latency: {self.std_dev_latency}"
            )


class IntermediateNodeTests:
    def __init__(self):
        self.df = pd.DataFrame()

    def append_row(self, new_df: pd.DataFrame):
        self.df = pd.concat([self.df, new_df], ignore_index=True)

    def calculate_stats(self) -> Test.Test_For_IntermediateNodes:
        avg_received_req = self.df['MeanReceivedRequests'].mean()
        avg_sent_req = self.df['MeanSentRequests'].mean()
        avg_f_qh = self.df['MeanForwardedQueryHits'].mean()
        avg_wasted_req_perc = self.df['MeanWastedRequests'].mean()
        avg_qh_efficiency = self.df['MeanQueryHitEfficiency'].mean()
        avg_disabled_nodes = self.df['CountDisabledNodes'].mean()
        avg_zero_work_done_nodes = self.df['CountZeroWorkDone'].mean()
        avg_total_work = self.df['TotalMessages'].mean()

        return Test.Test_For_IntermediateNodes(
            avg_received_req,
            avg_sent_req,
            avg_f_qh,
            avg_wasted_req_perc,
            avg_qh_efficiency,
            avg_disabled_nodes,
            avg_zero_work_done_nodes,
            avg_total_work
        )


class SrcNodeTests:
    def __init__(self):
        self.df = pd.DataFrame()

    def append_row(self, new_df: pd.DataFrame):
        self.df = pd.concat([self.df, new_df], ignore_index=True)

    def calculate_stats(self):
        avg_qh = self.df['QueryHits'].mean()
        median_qh = self.df['QueryHits'].median()
        max_qh = self.df['QueryHits'].max()
        avg_unique_qh = self.df['UniqueQueryHits'].mean()
        avg_total_retries = self.df['TotalRetries'].mean()
        avg_initialized_req = self.df['InitializedRequests'].mean()
        true_avg_success_rate = self.df['TrueSuccessRate'].mean()
        avg_success_rate = self.df['SuccessRate'].mean()
        avg_redundant_qh = self.df['RedundantQueryHits'].mean()
        avg_overhead_per_qh = self.df['OverheadPerQueryHit'].mean()

        return Test.Test_For_SourceNodes(
            avg_qh,
            median_qh,
            max_qh,
            avg_unique_qh,
            avg_total_retries,
            avg_initialized_req,
            true_avg_success_rate,
            avg_success_rate,
            avg_redundant_qh,
            avg_overhead_per_qh
        )


class AvgQueryHitsTests:
    def __init__(self):
        self.df = pd.DataFrame()

    def append_row(self, new_df: pd.DataFrame):
        self.df = pd.concat([self.df, new_df], ignore_index=True)

    def calculate_stats(self):

        avg_hops = self.df['AvgHops'].mean()
        avg_latency = self.df['AvgLatency'].mean()

        return Test.Test_For_AvgQueryHits(
            avg_hops,
            avg_latency
        )


class RawQueryHitsTests:
    def __init__(self):
        self.df = pd.DataFrame()

    def append_row(self, new_df: pd.DataFrame):
        if new_df.dropna(how='all').empty:
            return
        self.df = pd.concat([self.df, new_df], ignore_index=True)

    def calculate_stats(self):
        # "TotalQueryHits": len(query_hits_df),
        # "AvgHops": query_hits_df["Hops"].mean(),
        # "MinHops": query_hits_df["Hops"].min(),
        # "MaxHops": query_hits_df["Hops"].max(),
        # "MedianHops": query_hits_df["Hops"].median(),
        # "StdDevHops": query_hits_df["Hops"].std(),
        # "AvgLatency": query_hits_df["Seconds"].mean(),
        # "MinLatency": query_hits_df["Seconds"].min(),
        # "MaxLatency": query_hits_df["Seconds"].max(),
        # "MedianLatency": query_hits_df["Seconds"].median(),
        # "StdDevLatency": query_hits_df["Seconds"].std(),

        avg_hops = self.df['Hops'].mean()
        min_hops = self.df['Hops'].min()
        max_hops = self.df['Hops'].max()
        median_hops = self.df['Hops'].median()
        std_dev_hops = self.df['Hops'].std()

        avg_latency = self.df['Seconds'].mean()
        min_latency = self.df['Seconds'].min()
        max_latency = self.df['Seconds'].max()
        median_latency = self.df['Seconds'].median()
        std_dev_latency = self.df['Seconds'].std()

        return Test.Test_For_RawQueryHits(
            avg_hops,
            min_hops,
            max_hops,
            median_hops,
            std_dev_hops,
            avg_latency,
            min_latency,
            max_latency,
            median_latency,
            std_dev_latency
        )
