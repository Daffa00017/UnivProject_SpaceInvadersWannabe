using Microsoft.EntityFrameworkCore;
using Microsoft.OpenApi.Models;
using RestAPI.Data;
using System.Net;

var builder = WebApplication.CreateBuilder(args);


/*builder.WebHost.ConfigureKestrel(options =>
{
    options.ListenAnyIP(5209);                 // IPv4 0.0.0.0:5209
    options.Listen(IPAddress.IPv6Any, 5209);   // IPv6 [::]:5209
});*/
//  Listen on all interfaces (LAN) on port 5209
builder.WebHost.UseUrls("http://0.0.0.0:5209;http://[::]:5209");

builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen(c =>
{
    c.SwaggerDoc("v1", new OpenApiInfo { Title = "RestAPI", Version = "v1" });
});

// Absolute path so EF can open/create the file reliably
var dbPath = Path.Combine(builder.Environment.ContentRootPath, "leaderboard.db");
builder.Services.AddDbContext<LeaderboardDbContext>(opt =>
    opt.UseSqlite($"Data Source={dbPath}"));

//  CORS isn't needed for a desktop game, but keep it for browser tests
builder.Services.AddCors(o =>
{
    o.AddPolicy("GameClient", p => p
        .AllowAnyHeader()
        .AllowAnyMethod()
        .AllowAnyOrigin()); // dev-friendly; tighten later if you host public
});

var app = builder.Build();

#if DEBUG
app.UseDeveloperExceptionPage();
#endif

app.UseSwagger();
app.UseSwaggerUI();

//  In dev, don't force HTTPS redirection (avoids cert prompts on other PCs)
if (!app.Environment.IsDevelopment())
{
  //  app.UseHttpsRedirection();
}

app.UseCors("GameClient");
app.UseAuthorization();

app.MapControllers();

// Ensure DB is created at startup
using (var scope = app.Services.CreateScope())
{
    var db = scope.ServiceProvider.GetRequiredService<LeaderboardDbContext>();
    db.Database.EnsureCreated();
}

app.Run();
