# AI generated

# Network Node Performance Metrics

## 📊 Efficiency (`efficiency()`)

**Definition**:  
`(ForwardedQueryHits / SentRequests) × 100`

**What It Measures**:

- Percentage of forwarded requests that successfully returned results
- **100%** = All forwarded requests produced hits (optimal)
- **0%** = No forwarded requests yielded results

**Interpretation**:  
| Value Range | Performance Level | Suggested Action |
|-------------|-------------------|------------------|
| 90-100% | Excellent | Maintain current setup |
| 70-89% | Good | Minor optimizations |
| 50-69% | Fair | Investigate routing |
| <50% | Poor | Redesign forwarding logic |

**Example Calculation**:

```python
node = IntermediateNode(received=10, sent=8, forwarded=6)
print(f"{node.efficiency():.1f}%")  # Output: 75.0%
```

---

---

---

---

# Overhead Metric Analysis (`wasted_requests()`)

## 📌 Core Definition

**Formula**:  
`(SentRequests - ForwardedQueryHits) / ReceivedRequests × 100`  
Measures the percentage of received requests that were forwarded but failed to produce results.

## 🔍 Key Characteristics

- **Purpose**: Quantifies network resource waste from unproductive request forwarding
- **Ideal Value**: 0% (all forwarded requests yielded hits)
- **Worst Value**: 100% (all forwarded requests failed)
- **Calculation Basis**: Uses the node's request/hit counters from network operations

## 📊 Value Interpretation Guide

| Overhead %  | Performance Rating | Network Impact | Recommended Action       |
| ----------- | ------------------ | -------------- | ------------------------ |
| **0-10%**   | Excellent          | Negligible     | No action needed         |
| **11-25%**  | Good               | Minimal        | Routine monitoring       |
| **26-40%**  | Moderate           | Noticeable     | Check peer connections   |
| **41-60%**  | High               | Significant    | Optimize routing logic   |
| **61-100%** | Critical           | Severe         | Protocol redesign needed |

## 🌐 Network Implications

- **Low Overhead**: Indicates efficient request filtering and healthy network
- **High Overhead**: Suggests either:
  - Poor routing decisions
  - Sparse content distribution
  - Network topology issues

## 🛠️ Optimization Strategies

1. **For 20-40% Overhead**:
   - Implement TTL (Time-To-Live) for requests
   - Enhance peer selection algorithms
2. **For 40%+ Overhead**:
   - Introduce query caching
   - Redesign flood control mechanisms
   - Analyze network partitioning

**Note**: Always compare with efficiency metrics for complete performance analysis.
