class QueryHit:
    def __init__(self, id, hops, latency):
        self.id = id
        self.hops = hops
        self.latency = latency


class SrcNode:
    def __init__(self, id, tried_requests, initial_requests, query_hits):
        self.id = id
        # properties
        self.tried_requests = tried_requests
        self.initial_requests = initial_requests
        self.query_hits = query_hits
        self.query_hit_list: list = None
        # stats
        self.query_success_rate = (
            initial_requests / max(1, tried_requests)) * 100
        self.query_failure_rate = 100 - (self.query_success_rate/100)

    def setQueryHitList(self, queryHitList):
        self.query_hit_list = queryHitList

    def analyze_query_hits(self):

        if not self.query_hit_list:
            return None, None

        total_hops = 0
        total_latency = 0.0

        for hit in self.query_hit_list:
            total_hops += hit.hops
            total_latency += hit.latency

        return (
            total_hops / len(self.query_hit_list),
            total_latency / len(self.query_hit_list)
        )


class SinkNode:
    def __init__(self, id, received_requests):
        self.id = id
        self.received_requests = received_requests


class IntermediateNode:
    def __init__(self, id, received_requests, sent_requests, forwarded_hits):
        self.id = id
        self.received_requests = received_requests
        self.sent_requests = sent_requests
        self.forwarded_hits = forwarded_hits

    def wasted_requests(self):
        """Calculates overhead: (SentRequests - ForwardedHits) / ReceivedRequests
        It answers:
        "What fraction of requests this node handled were forwarded but led to NO results?"

        0% -> 0% overhead since everything sent was forwarded
        100% -> 100% overhead since nothing sent was forwarded
        """
        if self.received_requests == 0:
            return 0

        return (self.sent_requests - self.forwarded_hits) / max(1, self.received_requests) * 100

    def efficiency(self):
        """
        Calculates sent requests to query forwarding efficiency: ForwardedHits / SentRequests
        100- > 100% efficiency since everything sent was forwarded
        0 -> 0% efficiency since nothing sent was forwarded

        return 100 if no requests were sent
        """
        if self.sent_requests == 0:
            return 100

        return (self.forwarded_hits / self.sent_requests) * 100


class Test:

    '''-----------------------------------------------------------------'''

    class QueryHit_Per_Test_Average:
        def __init__(self, hops, latency):
            self.hops = hops or 0
            self.latency = latency or 0

    '''-----------------------------------------------------------------'''

    class SrcNode_Per_Test_Average:
        def __init__(self, initialzed_req, retried_req, query_hits):
            self.initialzed_req = initialzed_req
            self.retried_req = retried_req
            self.query_hits = query_hits

    '''-----------------------------------------------------------------'''
    class SinkNode_Per_Test_Average:
        def __init__(self, received):
            self.received = received

    '''-----------------------------------------------------------------'''

    class IntermediateNode_Per_Test_Average:
        def __init__(self, overhead, efficiency, sent, received, forwarded):
            self.sent = sent
            self.received = received
            self.forwarded = forwarded
            self.overhead = overhead
            self.efficiency = efficiency

    def __init__(self, src_node: SrcNode_Per_Test_Average,
                 sink_node: SinkNode_Per_Test_Average,
                 intermediate_nodes: IntermediateNode_Per_Test_Average,
                 query_hit: QueryHit_Per_Test_Average):
        self.src_node_tests = src_node
        self.sink_node_tests = sink_node
        self.intermediate_nodes_tests = intermediate_nodes
        self.query_hit_tests = query_hit

    def __str__(self):
        output = []

        # Query Hits section
        output.append("Query Hits:")
        output.append(f"    avg_hops = {self.query_hit_tests.hops:.2f}")
        output.append(f"    avg_latency = {self.query_hit_tests.latency:.6f}")
        output.append("")

        # Src Node section
        output.append("Src Node:")
        output.append(
            f"    initial_requests = {self.src_node_tests.initialzed_req}")
        output.append(
            f"    retried_requests = {self.src_node_tests.retried_req}")
        output.append(f"    query_hits = {self.src_node_tests.query_hits}")

        # Calculate and add success/failure rates
        success_rate = (self.src_node_tests.query_hits /
                        max(1, self.src_node_tests.initialzed_req)) * 100
        failure_rate = 100 - success_rate
        output.append(f"    query_success_rate = {success_rate:.2f}%")
        output.append(f"    query_failure_rate = {failure_rate:.2f}%")
        output.append("")

        # Sink Node section
        output.append("Sink Node:")
        output.append(
            f"    received_requests = {self.sink_node_tests.received}")
        output.append("")

        # Intermediate Nodes section
        output.append("Intermediate Nodes:")
        output.append(
            f"    received_requests = {self.intermediate_nodes_tests.received}")
        output.append(
            f"    sent_requests = {self.intermediate_nodes_tests.sent}")
        output.append(
            f"    forwarded_hits = {self.intermediate_nodes_tests.forwarded}")
        output.append(
            f"    overhead = {self.intermediate_nodes_tests.overhead:.2f}%")
        output.append(
            f"    efficiency = {self.intermediate_nodes_tests.efficiency:.2f}%")

        return "\n".join(output)


class AllTestsAveraged:
    def __init__(self, tests: list[Test]):
        self.tests = tests

    def calculate_averages(self):
        if not self.tests:
            return None

        # Initialize sums
        total_hops = 0.0
        total_latency = 0.0
        total_initial_requests = 0
        total_tried_requests = 0
        total_query_hits = 0
        total_success_rate = 0.0
        total_received_sink = 0
        total_received_inter = 0
        total_sent_inter = 0
        total_forwarded = 0
        total_overhead = 0.0
        total_efficiency = 0.0

        num_tests = len(self.tests)

        for test in self.tests:
            # Query Hits
            total_hops += test.query_hit_tests.hops
            total_latency += test.query_hit_tests.latency

            # Src Node
            total_initial_requests += test.src_node_tests.initialzed_req
            total_tried_requests += test.src_node_tests.retried_req
            total_query_hits += test.src_node_tests.query_hits
            success_rate = (test.src_node_tests.query_hits /
                            max(1, test.src_node_tests.initialzed_req)) * 100
            total_success_rate += success_rate

            # Sink Node
            total_received_sink += test.sink_node_tests.received

            # Intermediate Nodes
            total_received_inter += test.intermediate_nodes_tests.received
            total_sent_inter += test.intermediate_nodes_tests.sent
            total_forwarded += test.intermediate_nodes_tests.forwarded
            total_overhead += test.intermediate_nodes_tests.overhead
            total_efficiency += test.intermediate_nodes_tests.efficiency

        # Calculate averages
        avg_hops = total_hops / num_tests
        avg_latency = total_latency / num_tests
        avg_initial_requests = total_initial_requests / num_tests
        avg_tried_requests = total_tried_requests / num_tests
        avg_query_hits = total_query_hits / num_tests
        avg_success_rate = total_success_rate / num_tests
        avg_failure_rate = 100 - avg_success_rate
        avg_received_sink = total_received_sink / num_tests
        avg_received_inter = total_received_inter / num_tests
        avg_sent_inter = total_sent_inter / num_tests
        avg_forwarded = total_forwarded / num_tests
        avg_overhead = total_overhead / num_tests
        avg_efficiency = total_efficiency / num_tests

        # Create averaged Test object
        avg_query_hit = Test.QueryHit_Per_Test_Average(avg_hops, avg_latency)
        avg_src_node = Test.SrcNode_Per_Test_Average(
            avg_initial_requests, avg_tried_requests, avg_query_hits)
        avg_sink_node = Test.SinkNode_Per_Test_Average(avg_received_sink)
        avg_inter_node = Test.IntermediateNode_Per_Test_Average(
            avg_overhead, avg_efficiency, avg_sent_inter, avg_received_inter, avg_forwarded)

        return Test(avg_src_node, avg_sink_node, avg_inter_node, avg_query_hit)
