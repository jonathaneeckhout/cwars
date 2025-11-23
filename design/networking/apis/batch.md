# Batch

## POST /batch
Execute multiple commands in a single request.

**Request:**
```json
{
  "commands": [
    {
      "endpoint": "/units/move",
      "body": {
        "unit_ids": ["uuid1"],
        "target_position": {"x": 150, "y": 300}
      }
    },
    {
      "endpoint": "/buildings/construct",
      "body": {
        "building_type": "farm",
        "position": {"x": 120, "y": 270}
      }
    }
  ]
}
```

**Response:** `202 Accepted`
```json
{
  "batch_id": "uuid",
  "commands": [
    {
      "command_id": "uuid",
      "status": "pending"
    },
    {
      "command_id": "uuid",
      "status": "pending"
    }
  ]
}
```