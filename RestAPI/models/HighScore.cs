using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace RestAPI.Models
{
    public class HighScore
    {
        [Key] public int Id { get; set; }

        [Required, MaxLength(64)]
        public string PlayerName { get; set; } = "";

        // Optional (dev only)
        [MaxLength(64)]
        public string? Password { get; set; }

        public int Score { get; set; }
        public int Level { get; set; }

        // Keep as DateTime (UTC) to avoid SQLite DateTimeOffset ORDER BY issues
        public DateTime AchievedAt { get; set; }

        // For tie-breaker
        public double TimeSec { get; set; }

        [MaxLength(64)]
        public string SelectedShipId { get; set; } = "Classic";

        // Stored as JSON text
        public string? UnlockedShipsJson { get; set; }

        // Snapshot from client (e.g., GetTime())
        public double LastUpdatedSec { get; set; }
    }
}
