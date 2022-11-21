using ExamsAPI.Models;
using ExamsAPI.Services;
using Microsoft.AspNetCore.Http.HttpResults;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
// Learn more about configuring Swagger/OpenAPI at https://aka.ms/aspnetcore/swashbuckle
builder.Services.AddCors(options =>
{
    options.AddDefaultPolicy(
        policy =>
        {
            policy.AllowAnyOrigin().AllowAnyHeader().AllowAnyMethod();
        });
});
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();
var examsSonnectionString = builder.Configuration.GetConnectionString("Exams");
builder.Services.AddSingleton(new ExamServiceOptions { ConnectionString = examsSonnectionString });
builder.Services.AddTransient<IExamService, ExamService>();
var app = builder.Build();
app.UseCors();
// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.MapGet("/exams/", async (IExamService examService) =>
{
    return await examService.GetAsync();
})
.WithOpenApi();

app.MapGet("/exams/{examId}", async (int examId, IExamService examService) =>
{
    var exam = await examService.GetAsync(examId);
    if(exam == null)
    {
        return Results.NotFound();
    }

    return Results.Ok(exam);
})
.WithOpenApi();

app.MapDelete("/exams/{examId}", async (int examId, IExamService examService) =>
{
    await examService.DeleteAsync(examId);
})
.WithOpenApi();

app.MapPost("/exams/", async (Exam exam, IExamService examService) =>
{
    return await examService.CreateAsync(exam);
})
.WithOpenApi();

app.MapPut("/exams/", async (Exam exam, IExamService examService) =>
{
    await examService.UpdateAsync(exam);
})
.WithOpenApi();

app.Run();
