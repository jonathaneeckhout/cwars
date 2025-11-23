# World

## GET /world/scan
Scan for nearby entities from a position.

**Query Parameters:**
- `x`, `y`: Position to scan from
- `radius`: Scan radius (max 50)

**Response:** `200 OK`
```json
{
  "units": [
    {
      "unit_id": "uuid",
      "owner_id": "uuid",
      "unit_type": "scout",
      "position": {"x": 115, "y": 110}
    }
  ],
  "buildings": [
    {
      "building_id": "uuid",
      "owner_id": "uuid",
      "building_type": "farm",
      "position": {"x": 120, "y": 108}
    }
  ],
  "resources": [
    {
      "type": "forest",
      "position": {"x": 118, "y": 112}
    }
  ]
}
```